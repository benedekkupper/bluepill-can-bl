#include <span>
#include "arm/systick.hpp"
#include "board.hpp"
#include "can_bl/boot.hpp"
#include "can_bl/transport.hpp"
#include "stm32f1/can.hpp"
#include "stm32f1/flash.hpp"

using namespace can_bl;

static void can_init()
{
    RCC.APB1ENR.CANEN = 1; // enable CAN clock
    __DSB();
    can_pins_init();

    constexpr auto bit_timing = CAN_t::calc_bit_timing(can_peripheral_clock, CONFIG_CAN_BITRATE);
    static_assert(bit_timing.has_value(), "Invalid CAN bit timing");
    CAN.start(bit_timing.value());

    static constexpr CAN_t::filter_bank_list_32b_t filter_config = {
        CAN_t::filter_32b_t{.rtr = 0,
                            .ide = CONFIG_BLUEPILL_CAN_IDE,
                            .id = can_bl::updater_id << (CONFIG_BLUEPILL_CAN_IDE ? 0 : (29 - 11))},
        CAN_t::filter_32b_t{.rtr = 1,
                            .ide = CONFIG_BLUEPILL_CAN_IDE,
                            .id = can_bl::loader_id << (CONFIG_BLUEPILL_CAN_IDE ? 0 : (29 - 11))},
    };
    CAN.configure_filter_bank(0, filter_config);
}

auto app_flash_start()
{
    extern const uint8_t _app_flash_start[];
    return reinterpret_cast<uint32_t>(_app_flash_start);
}

auto app_flash_size()
{
    extern const uint8_t _bl_flash_start[];
    return *FLASH_SIZE_REG * 1024 -
           (app_flash_start() - reinterpret_cast<uint32_t>(_bl_flash_start));
}

struct app_header
{
    uint32_t msr;
    uint32_t reset_vector;

    bool is_valid() const
    {
        extern uint8_t _ram_start[];
        extern uint8_t _ram_end[];
        return (msr > reinterpret_cast<uint32_t>(_ram_start)) and
               (msr <= reinterpret_cast<uint32_t>(_ram_end)) and
               (reset_vector >= (app_flash_start() + sizeof(app_header))) and
               (reset_vector < (app_flash_start() + app_flash_size()));
    }
};

auto app_flash_header()
{
    return reinterpret_cast<const app_header*>(app_flash_start());
}

bool is_app_present()
{
    return app_flash_header()->is_valid();
}

[[noreturn]]
void boot_to_app()
{
    auto* app_header = app_flash_header();
    auto app_entry = reinterpret_cast<void (*)()>(app_header->reset_vector);
    __set_MSP(app_header->msr);
    app_entry();
}

inline constexpr uint16_t flash_page_size(uint32_t address = 0)
{
    return 1024;
}

can::frame info_frame(uint16_t current_chunk_index, uint16_t transfer_chunk_size,
                      error_flags errors)
{
    const uint16_t app_size_kB = app_flash_size() / 1024;
    return can::frame(
        can::frame_header{.id = loader_id, .ide = CONFIG_BLUEPILL_CAN_IDE, .rtr = false, .dlc = 8},
        info_payload::to_array(is_app_present(), errors, app_size_kB, transfer_chunk_size,
                               current_chunk_index));
}

uint32_t write_address(unsigned chunk_index)
{
    return app_flash_start() + chunk_index * flash_page_size();
}

int main()
{
    if (not pop_update_request() and is_app_present())
    {
        boot_to_app();
    }

    sys_clocks_init();
    can_init();

    // receive FW in chunks, erase and write to flash blocks
    uint16_t chunk_index = 0;
    error_flags current_errors = error_flags::none;
    app_header update_header;

    while (true)
    {
        CAN.flush_rx();

        CAN.send(info_frame(chunk_index, flash_page_size(), current_errors));
        current_errors = error_flags::none;

        payload_assembler<flash_page_size(), alignof(uint16_t)> rx_assembler;

        auto recv = CAN.receive(timeout(500));
        for (; recv.has_value(); recv = CAN.receive(timeout(500)))
        {
            if (recv->dlc == 0)
            {
                // restart transfer
                chunk_index = 0;
                break;
            }

            auto page_data = rx_assembler.append(std::span(recv->data.data(), recv->dlc));
            if (not page_data.has_value())
            {
                continue;
            }
            auto page_addr = write_address(chunk_index);
            auto transfer_size = page_data->size();
            if (chunk_index == 0)
            {
                update_header = *reinterpret_cast<const app_header*>(page_data->data());
                if ((transfer_size != flash_page_size()) or not update_header.is_valid())
                {
                    current_errors = error_flags::invalid_update;
                    break;
                }
                // the app header is left out from the first page write
                page_data = std::span(page_data->data() + sizeof(app_header),
                                      page_data->size() - sizeof(app_header));
                page_addr += sizeof(app_header);
            }
            else if ((write_address(chunk_index) + transfer_size) >
                     (app_flash_start() + app_flash_size()))
            {
                // too big update
                current_errors = error_flags::invalid_update;
                break;
            }

            FLASH.unlock();
            if (transfer_size > 0)
            {
                // erase page and write FW chunk to flash
                if (FLASH.page_erase(write_address(chunk_index)) and
                    FLASH.program(page_addr, reinterpret_cast<const uint16_t*>(page_data->data()),
                                  page_data->size()))
                {
                    chunk_index++;
                }
                else
                {
                    current_errors = error_flags::flash_operation;
                }
            }
            if ((transfer_size < flash_page_size()) and (current_errors == error_flags::none))
            {
                // short payload => transfer complete
                // write first two words (MSR and reset vector) at completion
                FLASH.program(app_flash_start(), reinterpret_cast<const uint16_t*>(&update_header),
                              sizeof(update_header));

                // try to boot it
                recv.reset();
            }
            FLASH.lock();
            break;
        }

        // when nothing was received, and app is present, boot to app
        if (not recv.has_value())
        {
            if (is_app_present())
            {
                CAN.stop();
#if 1
                NVIC_SystemReset();
#else
                boot_to_app();
#endif
            }
            current_errors = error_flags::rx_timeout;
        }
        if (current_errors != error_flags::none)
        {
            chunk_index = 0;
        }
    }
}

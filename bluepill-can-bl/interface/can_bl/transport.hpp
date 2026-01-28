#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include <span>
#include "can/frame.hpp"
#include <bitfilled.hpp>

namespace can_bl
{
enum error_flags : std::uint8_t
{
    none = 0,
    invalid_update = 1,
    flash_operation = 2,
    rx_timeout = 4,

    mask = 0x7F,
    // MSB is reserved:
    app_is_present = 0x80,
};

// make sure to link to can-bl-interface library in cmake
inline constexpr uint32_t updater_id = CONFIG_BLUEPILL_CAN_ID_PREFIX + 0;
inline constexpr uint32_t loader_id = CONFIG_BLUEPILL_CAN_ID_PREFIX + 1;

struct info_payload
{
    error_flags errors;
    std::uint16_t app_size_kB;
    std::uint16_t transfer_chunk_size;
    std::uint16_t current_chunk_index;

    static constexpr std::size_t frame_size() { return 8; }

    constexpr bool is_app_present() const { return bool(errors & app_is_present); }

    constexpr info_payload(std::span<const std::uint8_t, 8> data)
        : errors(static_cast<error_flags>(data[0])),
          app_size_kB((static_cast<std::uint16_t>(data[2]) << 8) | data[3]),
          transfer_chunk_size((static_cast<std::uint16_t>(data[4]) << 8) | data[5]),
          current_chunk_index((static_cast<std::uint16_t>(data[6]) << 8) | data[7])
    {}

    constexpr info_payload(error_flags errors_, std::uint16_t app_size_kB_,
                           std::uint16_t transfer_chunk_size_, std::uint16_t current_chunk_index_)
        : errors(errors_),
          app_size_kB(app_size_kB_),
          transfer_chunk_size(transfer_chunk_size_),
          current_chunk_index(current_chunk_index_)
    {}

    static constexpr std::array<std::uint8_t, 8> to_array(bool is_app_present, error_flags errors,
                                                          std::uint16_t app_size_kB,
                                                          std::uint16_t transfer_chunk_size,
                                                          std::uint16_t current_chunk_index)
    {
        return {std::uint8_t(errors | (is_app_present << 7)),
                0,
                std::uint8_t(app_size_kB >> 8),
                std::uint8_t(app_size_kB),
                std::uint8_t(transfer_chunk_size >> 8),
                std::uint8_t(transfer_chunk_size),
                std::uint8_t(current_chunk_index >> 8),
                std::uint8_t(current_chunk_index)};
    }
};

struct segmentation_header : public bitfilled::host_integer<std::uint8_t>
{
    BF_BITS(bool, 0, 0) first;
    BF_BITS(bool, 1, 1) last;
    BF_BITS(uint8_t, 2, 7) counter;
};

class payload_segmenter
{
    std::span<const std::uint8_t> data_;
    std::size_t position_{};

  public:
    constexpr payload_segmenter(const std::span<const std::uint8_t>& data) : data_(data) {}

    std::size_t next_segment(std::span<std::uint8_t> dest)
    {
        std::size_t chunk_space = dest.size() - sizeof(segmentation_header);
        if (dest.size() <= sizeof(segmentation_header) or (position_ > data_.size()))
        {
            return 0;
        }
        // set header
        segmentation_header header{};
        header.first = position_ == 0;
        header.last = position_ + chunk_space >= data_.size();
        header.counter = position_ / chunk_space; // requires constant dest size through all calls
        dest.front() = header;

        // fill data
        auto data_size = std::min<std::size_t>(chunk_space, data_.size() - position_);
        std::copy(data_.begin() + position_, data_.begin() + position_ + data_size,
                  dest.begin() + sizeof(segmentation_header));

        position_ += data_size + header.last; // allow zero-length data
        return sizeof(segmentation_header) + data_size;
    }
};

template <std::size_t SIZE, std::size_t ALIGN = 1>
class payload_assembler
{
    std::array<std::uint8_t, SIZE> buffer_ alignas(ALIGN);
    bitfilled::sized_unsigned_t<bitfilled::byte_width(SIZE)> position_{};
    std::uint8_t counter_{};

  public:
    payload_assembler() = default;

    void reset() { position_ = 0; }

    std::optional<std::span<const std::uint8_t>>
    append(const std::span<const std::uint8_t>& new_part)
    {
        std::optional<std::span<const std::uint8_t>> retval{};
        if (new_part.size() < sizeof(segmentation_header))
        {
            // too small pdu
            return retval;
        }
        segmentation_header header{new_part.front()};
        if (header.first)
        {
            position_ = 0;
        }
        else if ((counter_ != header.counter) or (position_ == 0))
        {
            // out of order packet or no start of packet
            return retval;
        }
        if ((position_ + new_part.size() - sizeof(segmentation_header)) > buffer_.size())
        {
            // overflow
            return retval;
        }
        counter_ = (header.counter + 1) & header.counter.mask();
        std::copy(new_part.begin() + sizeof(segmentation_header), new_part.end(),
                  buffer_.begin() + position_);
        position_ += new_part.size() - sizeof(segmentation_header);
        if (header.last)
        {
            retval.emplace(buffer_.data(), position_);
            position_ = 0;
        }
        return retval;
    }
};

} // namespace can_bl

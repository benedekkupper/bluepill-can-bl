#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <numeric>
#include <optional>
#include <span>
#include <vector>
#include "can_bl/transport.hpp"
#include <boost/ut.hpp>

int main(int argc, const char** argv)
{
    using namespace boost::ut;

    "info payload roundtrip"_test = []
    {
        constexpr auto errors =
            static_cast<can_bl::error_flags>(can_bl::invalid_update | can_bl::rx_timeout);
        constexpr auto raw = can_bl::info_payload::to_array(true, errors, 62, 1024, 7);
        constexpr can_bl::info_payload payload{std::span<const std::uint8_t, 8>{raw}};

        expect(payload.is_app_present());
        expect((std::uint8_t(payload.errors) & std::uint8_t(can_bl::mask)) == std::uint8_t(errors));
        expect(payload.app_size_kB == 62_u);
        expect(payload.transfer_chunk_size == 1024_u);
        expect(payload.current_chunk_index == 7_u);
    };

    "segmentation header roundtrip"_test = []
    {
        can_bl::segmentation_header header{};
        header.first = true;
        header.last = false;
        header.counter = 0x15;

        const auto raw = static_cast<std::uint8_t>(header);
        const can_bl::segmentation_header decoded{raw};

        expect(decoded.first);
        expect(not decoded.last);
        expect(decoded.counter == std::uint8_t{0x15});
        expect(raw == std::uint8_t{0x55});
    };

    "payload segmenter assembles full payload"_test = []
    {
        const std::array<std::uint8_t, 7> payload{0, 1, 2, 3, 4, 5, 6};
        can_bl::payload_segmenter segmenter{payload};
        can_bl::payload_assembler<payload.size()> assembler{};
        std::array<std::uint8_t, 4> segment_buffer{};
        std::optional<std::span<const std::uint8_t>> assembled{};

        for (std::uint8_t expected_counter = 0; not assembled; ++expected_counter)
        {
            const auto size = segmenter.next_segment(segment_buffer);
            expect(size > 0_u);

            const can_bl::segmentation_header header{segment_buffer.front()};
            expect(header.counter == expected_counter);
            expect(header.first == (expected_counter == 0));

            assembled =
                assembler.append(std::span<const std::uint8_t>{segment_buffer.data(), size});
            expect(assembled.has_value() == header.last);
        }

        expect(assembled.has_value());
        expect(assembled->size() == payload.size());
        expect(std::equal(assembled->begin(), assembled->end(), payload.begin()));
        expect(segmenter.next_segment(segment_buffer) == 0_u);
    };

    "payload segmenter and assembler handle counter rollover"_test = []
    {
        constexpr std::size_t payload_size = 70;

        std::array<std::uint8_t, payload_size> payload{};
        std::iota(payload.begin(), payload.end(), std::uint8_t{0});

        can_bl::payload_segmenter segmenter{payload};
        can_bl::payload_assembler<payload_size> assembler{};
        std::array<std::uint8_t, 2> segment_buffer{};
        std::optional<std::span<const std::uint8_t>> assembled{};
        std::size_t segment_index{};

        while (not assembled)
        {
            const auto size = segmenter.next_segment(segment_buffer);
            expect(size == segment_buffer.size());

            const can_bl::segmentation_header header{segment_buffer.front()};
            expect(header.first == (segment_index == 0));
            expect(header.last == (segment_index + 1 == payload.size()));
            expect(header.counter == std::uint8_t(segment_index & 0x3F));
            expect(segment_buffer[1] == payload[segment_index]);

            if (segment_index == 63)
            {
                expect(header.counter == std::uint8_t{63});
            }
            else if (segment_index == 64)
            {
                expect(header.counter == std::uint8_t{0});
            }
            else if (segment_index == 65)
            {
                expect(header.counter == std::uint8_t{1});
            }

            assembled =
                assembler.append(std::span<const std::uint8_t>{segment_buffer.data(), size});
            expect(assembled.has_value() == header.last);
            ++segment_index;
        }

        expect(segment_index == payload.size());
        expect(assembled.has_value());
        expect(assembled->size() == payload.size());
        expect(std::equal(assembled->begin(), assembled->end(), payload.begin()));
        expect(segmenter.next_segment(segment_buffer) == 0_u);
    };

    "payload assembler rejects out of order counters"_test = []
    {
        auto make_segment = [](bool first, bool last, std::uint8_t counter,
                               std::initializer_list<std::uint8_t> bytes)
        {
            std::vector<std::uint8_t> segment(1 + bytes.size());
            can_bl::segmentation_header header{};
            header.first = first;
            header.last = last;
            header.counter = counter;
            segment.front() = static_cast<std::uint8_t>(header);
            std::copy(bytes.begin(), bytes.end(), segment.begin() + 1);
            return segment;
        };

        can_bl::payload_assembler<8> assembler{};
        const auto first = make_segment(true, false, 0, {1, 2, 3});
        const auto wrong_counter = make_segment(false, true, 2, {4, 5});

        expect(not assembler.append(first).has_value());
        expect(not assembler.append(wrong_counter).has_value());
    };

    "payload assembler rejects overflow"_test = []
    {
        can_bl::payload_assembler<4> assembler{};
        can_bl::segmentation_header header{};
        header.first = true;
        header.last = true;
        header.counter = 0;

        const std::array<std::uint8_t, 6> oversized = {
            static_cast<std::uint8_t>(header), 1, 2, 3, 4, 5,
        };

        expect(not assembler.append(oversized).has_value());
    };

    return cfg<override>.run({.argc = argc, .argv = argv});
}

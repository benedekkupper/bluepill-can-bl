#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <span>

namespace can
{
struct frame_header
{
    std::uint32_t id;
    bool ide;
    bool rtr{};
    std::uint8_t dlc{}; // moved here for better alignment

    constexpr bool id_match(const frame_header& other, std::uint32_t bits_to_skip = 0) const
    {
        return (id & ~bits_to_skip) == (other.id & ~bits_to_skip) and ide == other.ide and
               rtr == other.rtr;
    }
};

struct frame : public frame_header
{
    std::array<std::uint8_t, 8> data alignas(std::uintptr_t);

    constexpr frame() = default;
    constexpr frame(frame_header fh) : frame_header{fh} {}
    constexpr frame(frame_header fh, const std::span<const std::uint8_t>& data) : frame{fh}
    {
        std::copy_n(data.data(), std::min<std::size_t>(dlc, this->data.size()), this->data.data());
    }
    constexpr frame(std::uint32_t id, bool ide, bool rtr, std::uint8_t dlc)
        : frame_header{id, ide, rtr, dlc}
    {}
    constexpr frame(std::uint32_t id, bool ide, bool rtr, std::uint8_t dlc,
                    const std::span<const std::uint8_t>& data)
        : frame{id, ide, rtr, dlc}
    {
        std::copy_n(data.data(), std::min<std::size_t>(dlc, this->data.size()), this->data.data());
    }
};
static_assert(sizeof(frame) == 16);

} // namespace can

#pragma once

#include <bit>
#include <cstdint>
#include <span>
#include <string>

namespace audiotag
{
constexpr std::uint16_t to_u16_be(std::uint8_t a, std::uint8_t b)
{
    return a << 8 | b;
}

constexpr std::uint16_t to_u16_le(std::uint8_t a, std::uint8_t b)
{
    return b << 8 | a;
}

constexpr std::uint16_t to_u16(const std::span<const std::byte> data)
{
    if constexpr(std::endian::native == std::endian::big)
    {
        return to_u16_be(std::to_integer<std::uint8_t>(data[0]), std::to_integer<std::uint8_t>(data[1]));
    }
    else
    {
        return to_u16_le(std::to_integer<std::uint8_t>(data[0]), std::to_integer<std::uint8_t>(data[1]));
    }
}

constexpr std::uint16_t to_u16_be(const std::span<const std::byte> data)
{
    return to_u16_be(std::to_integer<std::uint16_t>(data[0]), std::to_integer<std::uint16_t>(data[1]));
}

constexpr std::uint32_t to_u32_be(const std::span<const std::byte> data)
{
    return std::to_integer<std::uint32_t>(data[0]) << 24 | std::to_integer<std::uint32_t>(data[1]) << 16 |
           std::to_integer<std::uint32_t>(data[2]) << 8 | std::to_integer<std::uint32_t>(data[3]);
}

constexpr std::uint32_t to_synch_uint32_t(const std::span<const std::byte> data)
{
    std::uint32_t value{ 0 };
    for(std::size_t i = 0; i < 4; ++i)
    {
        value = (value << 7) | std::to_integer<std::uint32_t>(data[i]);
    }
    return value;
}

std::string from_latin1_to_utf8(std::span<const std::byte> data);
std::endian from_bom_to_endian(std::byte bom_0, std::byte bom_1);
std::u16string from_bytes_to_utf16(std::span<const std::byte> data, std::endian endianness);
std::u16string from_bytes_to_utf16(std::span<const std::byte> data);
} // namespace audiotag

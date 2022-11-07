#include <audiotag/byte_conversions.hpp>
#include <audiotag/byte_swap.hpp>

#include <stdexcept>

namespace audiotag
{
std::string from_latin1_to_utf8(const std::span<const std::byte> data)
{
    std::string out;
    out.reserve(data.size());

    for(const auto byte : data)
    {
        const auto character = std::to_integer<unsigned char>(byte);
        if(character < 0x80)
        {
            out.push_back(character);
        }
        else
        {
            out.push_back(0xc0 | character >> 6);
            out.push_back(0x80 | (character & 0x3f));
        }
    }

    return out;
}

std::endian from_bom_to_endian(std::byte bom_0, std::byte bom_1)
{
    if(bom_0 == std::byte{ 0xFF } && bom_1 == std::byte{ 0xFE })
    {
        return std::endian::little;
    }
    else if(bom_0 == std::byte{ 0xFE } && bom_1 == std::byte{ 0xFF })
    {
        return std::endian::big;
    }

    throw std::runtime_error("Invalid BOM");
}

template <bool byteswap> std::u16string from_bytes_to_utf16(const std::span<const std::byte> data)
{
    std::u16string out;
    out.reserve(data.size() / 2);

    for(std::size_t i = 0; i < data.size(); i += 2)
    {
        std::uint16_t u16_char = to_u16(data.subspan(i, 2));

        if constexpr(byteswap)
        {
            u16_char = audiotag::byteswap(u16_char);
        }

        out.push_back(u16_char);
    }

    return out;
}

std::u16string from_bytes_to_utf16(const std::span<const std::byte> data, std::endian endianness)
{
    if(endianness != std::endian::native)
    {
        return from_bytes_to_utf16<true>(data);
    }

    return from_bytes_to_utf16<false>(data);
}

std::u16string from_bytes_to_utf16(const std::span<const std::byte> data)
{
    if(data.size() < 2)
    {
        // Invalid string
        return u"";
    }

    const auto endianness = from_bom_to_endian(data[0], data[1]);
    const auto string_data = data.subspan(2);
    return from_bytes_to_utf16(string_data, endianness);
}
} // namespace audiotag

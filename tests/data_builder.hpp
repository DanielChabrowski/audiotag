#pragma once

#include <audiotag/byte_swap.hpp>

#include <algorithm>
#include <bit>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <span>
#include <string_view>
#include <vector>

namespace audiotag
{
class DataBuilder
{
public:
    void write(const std::string_view &data)
    {
        const auto buffer_size = buffer.size();
        buffer.resize(buffer_size + data.size());
        std::memcpy(buffer.data() + buffer_size, data.data(), data.size());
    }

    void write(const std::u16string_view &data, std::endian endianess)
    {
        for(const auto &character : data)
        {
            write(static_cast<std::uint16_t>(character), endianess);
        }
    }

    void write(const std::span<const std::uint8_t> &data)
    {
        const auto buffer_size = buffer.size();
        buffer.resize(buffer_size + data.size());
        std::memcpy(buffer.data() + buffer_size, data.data(), data.size());
    }

    void write(const std::span<const std::byte> &data)
    {
        const auto buffer_size = buffer.size();
        buffer.resize(buffer_size + data.size());
        std::memcpy(buffer.data() + buffer_size, data.data(), data.size());
    }

    void write(std::byte byte, std::size_t count)
    {
        const auto buffer_size = buffer.size();
        buffer.resize(buffer_size + count);
        std::fill_n(buffer.data() + buffer_size, count, byte);
    }

    void write(std::uint32_t value, std::endian endianess)
    {
        if(endianess != std::endian::native)
        {
            value = byteswap(value);
        }

        const std::uint32_t data[1] = { value };
        write(std::as_bytes(std::span(data)));
    }

    void write(std::uint16_t value, std::endian endianess)
    {
        if(endianess != std::endian::native)
        {
            value = byteswap(value);
        }

        const std::uint16_t data[1] = { value };
        write(std::as_bytes(std::span(data)));
    }

    void write_synch_safe(std::uint32_t value)
    {
        write(std::byte(value >> 21 & 0x7F), 1);
        write(std::byte(value >> 14 & 0x7F), 1);
        write(std::byte(value >> 7 & 0x7F), 1);
        write(std::byte(value & 0x7F), 1);
    }

    void write_padded(const std::string_view data, std::uint32_t size)
    {
        assert(data.size() <= size);

        write(data);
        write(std::byte{ 0 }, size - data.size());
    }

    [[nodiscard]] std::vector<std::byte> build()
    {
        return buffer;
    }

private:
    std::vector<std::byte> buffer;
};
} // namespace audiotag

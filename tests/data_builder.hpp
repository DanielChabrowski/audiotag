#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <span>
#include <string>
#include <vector>

namespace audiotag
{
class DataBuilder
{
public:
    void write(const std::string &data)
    {
        const auto buffer_size = buffer.size();
        buffer.resize(buffer_size + data.size());
        std::memcpy(buffer.data() + buffer_size, data.data(), data.size());
    }

    void write(const std::span<std::uint8_t> &data)
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

    std::vector<std::byte> build()
    {
        return buffer;
    }

private:
    std::vector<std::byte> buffer;
};
} // namespace audiotag

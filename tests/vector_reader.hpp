#pragma once

#include <audiotag/reader.hpp>

#include <cstddef>
#include <cstring>
#include <vector>

namespace audiotag
{
class VectorReader : public Reader
{
public:
    using DataVec = std::vector<unsigned char>;

    explicit VectorReader(const DataVec &data)
    : data{ data }
    {
    }

    VectorReader(const VectorReader &) = delete;
    VectorReader &operator=(const VectorReader &) = delete;

    std::size_t length() const override
    {
        return data.size();
    }

    std::size_t buffer_size() const override
    {
        return 1024;
    }

    std::size_t read(std::span<std::byte> buffer) override
    {
        const auto buffer_size = buffer.size();
        const auto read_size = std::min(data.size() - cursor, cursor + buffer_size);

        std::memcpy(buffer.data(), data.data() + cursor, read_size);
        cursor += read_size;

        return read_size;
    }

    bool seek(long offset) override
    {
        cursor = offset;
        return true;
    }

private:
    const DataVec &data;
    std::ptrdiff_t cursor{ 0 };
};
} // namespace audiotag

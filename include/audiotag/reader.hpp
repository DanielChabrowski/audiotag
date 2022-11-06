#pragma once

#include <cstddef>
#include <span>

namespace audiotag
{
class Reader
{
public:
    virtual ~Reader() = default;

    [[nodiscard]] virtual std::size_t length() const = 0;
    [[nodiscard]] virtual std::size_t buffer_size() const = 0;

    [[nodiscard]] virtual std::size_t read(std::span<std::byte> buffer) = 0;

    [[nodiscard]] virtual bool seek(long offset) = 0;
};
} // namespace audiotag

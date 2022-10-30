#pragma once

#include <memory>
#include <span>
#include <string_view>

namespace audiotag
{
class FileReader
{
public:
    explicit FileReader(std::string_view filename);
    ~FileReader();

    std::size_t length() const;
    std::size_t buffer_size() const;

    std::size_t read(std::span<std::byte> buffer);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
} // namespace audiotag

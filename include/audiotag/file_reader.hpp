#pragma once

#include <audiotag/reader.hpp>

#include <memory>
#include <span>
#include <string_view>

namespace audiotag
{
class FileReader : public Reader
{
public:
    explicit FileReader(std::string_view filename);
    ~FileReader();

    std::size_t length() const override;
    std::size_t buffer_size() const override;

    std::size_t read(std::span<std::byte> buffer) override;

    bool seek(long offset) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
} // namespace audiotag

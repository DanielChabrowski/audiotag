#include "audiotag/file_reader.hpp"

#include <sys/stat.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace audiotag
{
struct FileReader::Impl
{
    std::FILE *file{ nullptr };
    std::size_t file_size{ 0 };
    std::size_t buffer_size{ 0 };

    ~Impl()
    {
        if(file)
        {
            std::fclose(file);
        }
    }
};

FileReader::FileReader(std::string_view filename)
: impl(std::make_unique<Impl>())
{
    std::cout << "opening " << filename << std::endl;
    impl->file = std::fopen(filename.data(), "rb");
    if(!impl->file)
    {
        throw std::runtime_error("File not opened");
    }

    auto fd = fileno(impl->file);

    struct stat file_stat = {};
    if(const auto stat_result = fstat(fd, &file_stat); stat_result != 0)
    {
        throw std::runtime_error("Couldn't read file stat");
    }

    impl->file_size = file_stat.st_size;
    impl->buffer_size = file_stat.st_blksize;
}

FileReader::~FileReader() = default;

std::size_t FileReader::length() const
{
    return impl->file_size;
}

std::size_t FileReader::buffer_size() const
{
    return impl->buffer_size;
}

std::size_t FileReader::read(std::span<std::byte> buffer)
{
    return std::fread(buffer.data(), sizeof(std::byte), buffer.size(), impl->file);
}
} // namespace audiotag

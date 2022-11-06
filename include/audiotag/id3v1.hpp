#pragma once

#include <cstddef>
#include <string>

namespace audiotag::ID3v1
{
constexpr std::byte Identifier[3] = {
    std::byte{ 'T' },
    std::byte{ 'A' },
    std::byte{ 'G' },
};

struct Tags
{
    std::string title;
    std::string artist;
    std::string album;
    std::string year;
    std::string comment;
    std::uint8_t track;
    std::uint8_t genre;
};
} // namespace audiotag::ID3v1

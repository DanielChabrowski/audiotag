#pragma once

#include <audiotag/tag.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace audiotag::ID3v2
{
constexpr std::byte Identifier[3] = {
    std::byte{ 'I' },
    std::byte{ 'D' },
    std::byte{ '3' },
};

struct Header
{
    std::uint8_t version_major{ 0 };
    std::uint8_t version_revision{ 0 };
    bool unsynchronization{ false };
    bool extended_header{ false };
    bool experimental{ false };
    std::uint32_t size{ 0 };
};

struct TagFrame
{
    std::array<std::byte, 4> id;
    std::vector<std::byte> data;
    bool tag_preservation{ false };
    bool file_preservation{ false };
    bool read_only{ false };
    bool compression{ false };
    bool encryption{ false };
    bool grouping_identity{ false };
};

class Tags
{
public:
    explicit Tags(Header &&header, std::vector<TagFrame> &&frames);

    const std::vector<TagFrame> &getFrames() const;

    std::string getStringValue(Tag tag_name) const;

private:
    Header header;
    std::vector<TagFrame> frames;
};
} // namespace audiotag::ID3v2

#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace audiotag
{
class FileReader;

enum class Tag
{
    TITLE,
    ARIST,
    ALBUM,
    DISCNUMBER,
    TRACKNUMBER,
};

enum class Encoding : int
{
    Latin1 = 0,
    UTF16 = 1,
    UTF16_BE = 2,
    UTF8 = 3,
};

namespace ID3v1
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
} // namespace ID3v1

namespace ID3v2
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
} // namespace ID3v2

class MpegFile
{
public:
    MpegFile(FileReader &reader);

    const std::optional<ID3v1::Tags> &id3v1();
    const std::optional<ID3v2::Tags> &id3v2();

private:
    std::optional<ID3v2::Tags> read_id3v2(FileReader &reader);
    std::optional<ID3v1::Tags> read_id3v1(FileReader &reader);

private:
    std::optional<ID3v1::Tags> id3v1_tags;
    std::optional<ID3v2::Tags> id3v2_tags;
};
} // namespace audiotag

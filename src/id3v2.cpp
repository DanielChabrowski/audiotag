#include <audiotag/byte_conversions.hpp>
#include <audiotag/byte_swap.hpp>
#include <audiotag/id3v2.hpp>
#include <frozen/map.h>
#include <utf8/cpp17.h>

#include <algorithm>
#include <bit>
#include <span>

namespace audiotag::ID3v2
{
Tags::Tags(Header &&header, std::vector<TagFrame> &&frames)
: header{ header }
, frames{ frames }
{
}

const std::vector<TagFrame> &Tags::getFrames() const
{
    return frames;
}

static const constinit frozen::map<Tag, std::array<std::byte, 4>, 5> tag_mapping = {
    {
        Tag::TITLE,
        {
            std::byte{ 'T' },
            std::byte{ 'I' },
            std::byte{ 'T' },
            std::byte{ '2' },
        },
    },
    {
        Tag::ARIST,
        {
            std::byte{ 'T' },
            std::byte{ 'P' },
            std::byte{ 'E' },
            std::byte{ '1' },
        },
    },
    {
        Tag::ALBUM,
        {
            std::byte{ 'T' },
            std::byte{ 'A' },
            std::byte{ 'L' },
            std::byte{ 'B' },
        },
    },
    {
        Tag::TRACKNUMBER,
        {
            std::byte{ 'T' },
            std::byte{ 'R' },
            std::byte{ 'C' },
            std::byte{ 'K' },
        },
    },
    {
        Tag::DISCNUMBER,
        {
            std::byte{ 'T' },
            std::byte{ 'P' },
            std::byte{ 'O' },
            std::byte{ 'S' },
        },
    },
};

std::string Tags::getStringValue(Tag tag) const
{
    const auto frame_tag = tag_mapping.at(tag);
    const auto frameIt = std::find_if(frames.cbegin(), frames.cend(),
        [&frame_tag](const auto &frame) { return frame.id == frame_tag; });

    if(frameIt != frames.cend() && !frameIt->data.empty())
    {
        const auto encoding{ std::to_integer<std::uint8_t>(frameIt->data[0]) };
        if(encoding == 0)
        {
            return from_latin1_to_utf8(std::span<const std::byte>(frameIt->data).subspan(1));
        }
        else if(encoding == 1)
        {
            return utf8::utf16to8(from_bytes_to_utf16(std::span<const std::byte>(frameIt->data).subspan(1)));
        }
        else if(encoding == 2)
        {
            return utf8::utf16to8(from_bytes_to_utf16(
                std::span<const std::byte>(frameIt->data).subspan(1), std::endian::big));
        }
    }

    return "";
}
} // namespace audiotag::ID3v2

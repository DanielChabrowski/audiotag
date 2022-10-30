#include <audiotag/byte_swap.hpp>
#include <audiotag/file_reader.hpp>
#include <audiotag/mpeg/mpeg_file.hpp>
#include <frozen/map.h>
#include <utf8/cpp17.h>

#include <algorithm>
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

constexpr std::uint16_t to_u16_be(std::uint8_t a, std::uint8_t b)
{
    return a << 8 | b;
}

constexpr std::uint16_t to_u16_be(const std::span<const std::byte> data)
{
    return to_u16_be(std::to_integer<std::uint16_t>(data[0]), std::to_integer<std::uint16_t>(data[1]));
}

constexpr std::uint16_t to_u16_native(const std::span<const std::byte> data)
{
    const auto u16 = to_u16_be(data);
    return std::endian::native == std::endian::little ? audiotag::byteswap(u16) : u16;
}

constexpr std::uint32_t to_u32_be(const std::span<const std::byte> data)
{
    return std::to_integer<std::uint32_t>(data[0]) << 24 | std::to_integer<std::uint32_t>(data[1]) << 16 |
           std::to_integer<std::uint32_t>(data[2]) << 8 | std::to_integer<std::uint32_t>(data[3]);
}

std::string from_latin1_to_utf8(const std::span<const std::byte> data)
{
    std::string out;
    out.reserve(data.size());

    for(const auto byte : data)
    {
        const auto character = std::to_integer<unsigned char>(byte);
        if(character < 0x80)
        {
            out.push_back(character);
        }
        else
        {
            out.push_back(0xc0 | character >> 6);
            out.push_back(0x80 | (character & 0x3f));
        }
    }

    return out;
}

std::endian from_bom_to_endian(const std::span<const std::byte> bom_data)
{
    const auto bom = to_u16_native(bom_data);

    if(bom == 0xFFFE)
    {
        return std::endian::little;
    }
    else if(bom == 0xFEFF)
    {
        return std::endian::big;
    }

    throw std::runtime_error("Invalid BOM");
}

std::u16string from_bytes_to_utf16(const std::span<const std::byte> data)
{
    std::u16string out;
    out.reserve(data.size() - 2);

    if(data.size() < 2)
    {
        // Invalid string
        return out;
    }

    const auto endian = from_bom_to_endian(data.subspan(0, 2));
    const auto string_data = data.subspan(2);

    for(std::size_t i = 0; i < string_data.size(); i += 2)
    {
        std::uint16_t u16_char = to_u16_be(std::to_integer<std::uint16_t>(string_data[i]),
            std::to_integer<std::uint16_t>(string_data[i + 1]));

        if(endian != std::endian::native)
        {
            u16_char = audiotag::byteswap(u16_char);
        }

        out.push_back(u16_char);
    }

    return out;
}

constexpr std::uint32_t to_synch_uint32_t(std::span<std::byte> data)
{
    std::uint32_t value{ 0 };
    for(std::size_t i = 0; i < 4; ++i)
    {
        value = (value << 7) | std::to_integer<std::uint32_t>(data[i]);
    }
    return value;
}

constexpr std::uint32_t to_uint32_t(const std::span<const std::byte> data)
{
    return std::to_integer<std::uint32_t>(data[0]) | std::to_integer<std::uint32_t>(data[1]) << 8 |
           std::to_integer<std::uint32_t>(data[2]) << 16 | std::to_integer<std::uint32_t>(data[3]) << 24;
}

namespace audiotag
{
namespace ID3v2
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

struct elsa
{
    constexpr std::size_t operator()(const Tag &value, std::size_t) const
    {
        return static_cast<std::size_t>(value);
    }
};

static constinit frozen::map<Tag, std::array<std::byte, 4>, 5> tag_mapping = {
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
        else
        {
            return utf8::utf16to8(from_bytes_to_utf16(std::span<const std::byte>(frameIt->data).subspan(1)));
        }
    }

    return "";
}
} // namespace ID3v2

MpegFile::MpegFile(audiotag::FileReader &reader)
{
    constexpr std::size_t header_size{ 10 };
    std::byte header[header_size]{};

    const std::size_t header_bytes_read = reader.read(header);
    assert(header_bytes_read == header_size);

    auto header_span = std::span(header);
    const auto header_tag = header_span.subspan(0, 3);
    if(std::memcmp(ID3v2::Identifier, header_tag.data(), header_tag.size()))
    {
        return;
    }

    const auto version_span = header_span.subspan(3, 2);
    const auto version_major = std::to_integer<std::uint8_t>(version_span[0]);
    const auto version_revision = std::to_integer<std::uint8_t>(version_span[1]);

    const auto synch_safe_size = version_major >= 4;
    const auto flags_span = header_span.subspan(5, 1);

    const auto size = header_span.subspan(6, 4);
    const auto synch_size = to_synch_uint32_t(size);

    // make sure synch_size is smaller than file size
    assert(synch_size < reader.length());

    std::vector<std::byte> frames(synch_size);

    const std::size_t frames_bytes_read = reader.read(frames);
    assert(frames_bytes_read == synch_size);

    const auto frames_span = std::span(frames);

    struct SpanFrame
    {
        std::array<std::byte, 4> id;
        std::uint16_t flags;
        std::span<std::byte> data_span;
    };

    std::vector<SpanFrame> span_frames;

    std::size_t offset{ 0 };
    while(offset < synch_size)
    {
        const auto frame_id = frames_span.subspan(offset, 4);
        if(frame_id[0] == std::byte{ '\0' })
        {
            break;
        }

        const auto frame_size_span = frames_span.subspan(offset + 4, 4);
        const auto frame_size =
            synch_safe_size ? to_synch_uint32_t(frame_size_span) : to_u32_be(frame_size_span);
        const auto frame_flags = frames_span.subspan(offset + 8, 2);
        const auto frame_flags_int = to_u16_be(frame_flags);

        span_frames.emplace_back(SpanFrame{
            .id = { frame_id[0], frame_id[1], frame_id[2], frame_id[3] },
            .flags = frame_flags_int,
            .data_span = frames_span.subspan(offset + 10, frame_size),
        });

        offset += frame_size + 10;
    }

    std::vector<ID3v2::TagFrame> tag_frames;
    tag_frames.reserve(span_frames.size());

    for(const auto &span_frame : span_frames)
    {
        std::vector<std::byte> frame_data(span_frame.data_span.size());

        std::memcpy(frame_data.data(), span_frame.data_span.data(), span_frame.data_span.size());

        tag_frames.emplace_back(ID3v2::TagFrame{
            .id = span_frame.id,
            .data = std::move(frame_data),
        });
    }

    id3v2_tags = ID3v2::Tags(ID3v2::Header{}, std::move(tag_frames));
}

const std::optional<ID3v1::Tags> &MpegFile::id3v1()
{
    return id3v1_tags;
}

const std::optional<ID3v2::Tags> &MpegFile::id3v2()
{
    return id3v2_tags;
}
} // namespace audiotag

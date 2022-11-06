#include <audiotag/byte_conversions.hpp>
#include <audiotag/mpeg/mpeg_file.hpp>
#include <audiotag/reader.hpp>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace audiotag
{
MpegFile::MpegFile(audiotag::Reader &reader)
{
    id3v2_tags = read_id3v2(reader);
    id3v1_tags = read_id3v1(reader);
}

const std::optional<ID3v1::Tags> &MpegFile::id3v1()
{
    return id3v1_tags;
}

const std::optional<ID3v2::Tags> &MpegFile::id3v2()
{
    return id3v2_tags;
}

std::optional<ID3v2::Tags> MpegFile::read_id3v2(Reader &reader)
{
    constexpr std::size_t header_size{ 10 };
    std::byte header[header_size]{};

    const std::size_t header_bytes_read = reader.read(header);
    if(header_bytes_read != header_size)
    {
        return std::nullopt;
    }

    auto header_span = std::span(header);
    const auto header_tag = header_span.subspan(0, 3);
    if(std::memcmp(ID3v2::Identifier, header_tag.data(), header_tag.size()))
    {
        return std::nullopt;
    }

    const auto version_span = header_span.subspan(3, 2);
    const auto version_major = std::to_integer<std::uint8_t>(version_span[0]);
    const auto version_revision = std::to_integer<std::uint8_t>(version_span[1]);

    const auto synch_safe_size = version_major >= 4;
    const auto flags_span = header_span.subspan(5, 1);

    const auto size = header_span.subspan(6, 4);
    const auto synch_size = to_synch_uint32_t(size);

    // make sure synch_size is smaller than file size
    if(synch_size >= reader.length())
    {
        return std::nullopt;
    }

    std::vector<std::byte> frames(synch_size);

    const std::size_t frames_bytes_read = reader.read(frames);
    if(frames_bytes_read != synch_size)
    {
        return std::nullopt;
    }

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

        constexpr auto frame_header_size = 10u;

        span_frames.emplace_back(SpanFrame{
            .id = { frame_id[0], frame_id[1], frame_id[2], frame_id[3] },
            .flags = frame_flags_int,
            .data_span = frames_span.subspan(offset + frame_header_size, frame_size),
        });

        offset += frame_size + frame_header_size;
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

    return ID3v2::Tags(
        ID3v2::Header{
            .version_major = version_major,
            .version_revision = version_revision,
        },
        std::move(tag_frames));
}

std::optional<ID3v1::Tags> MpegFile::read_id3v1(Reader &reader)
{
    constexpr auto id3v1_tag_size = 128u;

    std::vector<std::byte> buffer(id3v1_tag_size);

    reader.seek(reader.length() - id3v1_tag_size);

    const auto bytes_read = reader.read(buffer);
    if(bytes_read != id3v1_tag_size)
    {
        return std::nullopt;
    }

    if(std::memcmp(ID3v1::Identifier, buffer.data(), sizeof(ID3v1::Identifier)))
    {
        return std::nullopt;
    }

    const auto tags = std::span(buffer);
    const auto is_id3v11 = (tags[125] == std::byte{ 0 } && tags[126] != std::byte{ 0 });
    const auto comment_size = is_id3v11 ? 28 : 30;

    return ID3v1::Tags{
        .title = from_latin1_to_utf8(tags.subspan(3, 30)).data(),
        .artist = from_latin1_to_utf8(tags.subspan(33, 30)).data(),
        .album = from_latin1_to_utf8(tags.subspan(63, 30)).data(),
        .year = from_latin1_to_utf8(tags.subspan(93, 4)).data(),
        .comment = from_latin1_to_utf8(tags.subspan(97, comment_size)).data(),
        .track = is_id3v11 ? std::to_integer<std::uint8_t>(tags[126]) : std::uint8_t{ 0 },
        .genre = std::to_integer<std::uint8_t>(tags[127]),
    };
}
} // namespace audiotag

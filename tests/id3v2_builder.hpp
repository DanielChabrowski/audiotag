#pragma once

#include "data_builder.hpp"

#include <audiotag/encoding.hpp>

#include <bit>
#include <cstdint>
#include <string>

namespace audiotag
{
class ID3v2Builder : private DataBuilder
{
public:
    struct FrameHeader
    {
        const char *frame_id;
        std::uint16_t flags;
    };

    void add_text_information_frame(FrameHeader header, std::string_view text)
    {
        write_frame_header(header, text.size() + 1);
        write(std::byte{ 0 }, 1); // latin1 encoding
        write(text);
    }

    void add_text_information_frame(FrameHeader header, std::u16string_view text, Encoding encoding, std::endian endianness)
    {
        const auto needs_bom = (encoding == Encoding::UTF16); // utf-16

        write_frame_header(header, text.size() * 2 + 1 + (needs_bom * 2));
        write(static_cast<std::byte>(encoding), 1);

        if(needs_bom)
        {
            if(std::endian::big == endianness)
            {
                write(std::byte{ 0xFE }, 1); // bom
                write(std::byte{ 0xFF }, 1); // bom
            }
            else
            {
                write(std::byte{ 0xFF }, 1); // bom
                write(std::byte{ 0xFE }, 1); // bom
            }
        }

        write(text, endianness);
    }

    void write_frame_header(FrameHeader &header, std::uint32_t size)
    {
        write(std::string(header.frame_id));
        write_synch_safe(size);
        write(header.flags, std::endian::big);
    }

    [[nodiscard]] std::vector<std::byte> build()
    {
        return DataBuilder::build();
    }
};
} // namespace audiotag

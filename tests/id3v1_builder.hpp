#pragma once

#include "data_builder.hpp"

#include <audiotag/mpeg/mpeg_file.hpp>

namespace audiotag
{
struct ID3v1Builder
{
    static std::vector<std::byte> build(ID3v1::Tags tags)
    {
        auto builder = DataBuilder{};
        builder.write(ID3v1::Identifier);
        builder.write_padded(tags.title, 30);
        builder.write_padded(tags.artist, 30);
        builder.write_padded(tags.album, 30);
        builder.write_padded(tags.year, 4);
        builder.write_padded(tags.comment, 29);
        builder.write(std::byte{ tags.track }, 1);
        builder.write(std::byte{ tags.genre }, 1);
        return builder.build();
    }
};
} // namespace audiotag

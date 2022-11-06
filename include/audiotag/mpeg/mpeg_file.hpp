#pragma once

#include <audiotag/id3v1.hpp>
#include <audiotag/id3v2.hpp>

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace audiotag
{
class Reader;

class MpegFile
{
public:
    MpegFile(Reader &reader);

    const std::optional<ID3v1::Tags> &id3v1();
    const std::optional<ID3v2::Tags> &id3v2();

private:
    std::optional<ID3v2::Tags> read_id3v2(Reader &reader);
    std::optional<ID3v1::Tags> read_id3v1(Reader &reader);

private:
    std::optional<ID3v1::Tags> id3v1_tags;
    std::optional<ID3v2::Tags> id3v2_tags;
};
} // namespace audiotag

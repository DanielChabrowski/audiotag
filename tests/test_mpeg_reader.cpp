#include <audiotag/file_reader.hpp>
#include <audiotag/mpeg/mpeg_file.hpp>

#include <iostream>
#include <ostream>

//
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

using namespace audiotag;

inline std::ostream &operator<<(std::ostream &os, const std::array<std::byte, 4> &data)
{
    for(const auto byte : data)
    {
        os << std::to_integer<char>(byte);
    }
    return os;
}

TEST_CASE("MpegFileWithoutTags")
{
    FileReader reader{ TEST_DATA_DIR "/no_tags.mp3" };
    MpegFile mpeg{ reader };

    REQUIRE_FALSE(mpeg.id3v1());
    REQUIRE_FALSE(mpeg.id3v2());
}

TEST_CASE("MpegFileWithApeTags")
{
    FileReader reader{ TEST_DATA_DIR "/ape.mp3" };
    MpegFile mpeg{ reader };

    REQUIRE_FALSE(mpeg.id3v1());
    REQUIRE_FALSE(mpeg.id3v2());
}

TEST_CASE("MpegFileWithID3v1Tags")
{
    FileReader reader{ TEST_DATA_DIR "/id3v1_only.mp3" };
    MpegFile mpeg{ reader };

    REQUIRE_FALSE(mpeg.id3v1());
    REQUIRE_FALSE(mpeg.id3v2());
}

TEST_CASE("MpegFileWithID3v1Andv2Tags")
{
    FileReader reader{ TEST_DATA_DIR "/id3v2_id3v1.mp3" };
    MpegFile mpeg{ reader };

    REQUIRE_FALSE(mpeg.id3v1());
    REQUIRE(mpeg.id3v2());
}

TEST_CASE("MpegFileWithID3v2TagsOnly")
{
    FileReader reader{ TEST_DATA_DIR "/id3v2_only.mp3" };
    MpegFile mpeg{ reader };

    const auto v2_tags = mpeg.id3v2();
    REQUIRE(v2_tags);

    std::cout << "ARTIST " << v2_tags->getStringValue(Tag::ARIST) << std::endl;
    std::cout << "TITLE " << v2_tags->getStringValue(Tag::TITLE) << std::endl;
    std::cout << "ALBUM " << v2_tags->getStringValue(Tag::ALBUM) << std::endl;
    std::cout << "TRACKNUMBER " << v2_tags->getStringValue(Tag::TRACKNUMBER) << std::endl;
    std::cout << "DISCNUMBER " << v2_tags->getStringValue(Tag::DISCNUMBER) << std::endl;

    const auto frames = v2_tags->getFrames();
    for(const auto &frame : frames)
    {
        std::cout << "Frame " << frame.id << " size: " << frame.data.size() << std::endl;
    }
}

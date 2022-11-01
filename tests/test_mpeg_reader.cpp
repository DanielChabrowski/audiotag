#include <audiotag/file_reader.hpp>
#include <audiotag/mpeg/mpeg_file.hpp>

//
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

using namespace audiotag;

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

    REQUIRE_FALSE(mpeg.id3v2());

    const auto tags = mpeg.id3v1();
    REQUIRE(tags);

    CHECK(tags->title == "Sample title");
    CHECK(tags->artist == "Sample artist");
    CHECK(tags->album == "Sample album");
    CHECK(tags->year == "");
    CHECK(tags->comment == "");
    CHECK(tags->track == 3);
    // REQUIRE(tags->genre == ??);
}

TEST_CASE("MpegFileWithID3v1Andv2Tags")
{
    FileReader reader{ TEST_DATA_DIR "/id3v2_id3v1.mp3" };
    MpegFile mpeg{ reader };

    REQUIRE(mpeg.id3v1());
    REQUIRE(mpeg.id3v2());
}

TEST_CASE("MpegFileWithID3v2TagsOnly")
{
    FileReader reader{ TEST_DATA_DIR "/id3v2_only.mp3" };
    MpegFile mpeg{ reader };

    const auto tags = mpeg.id3v2();
    REQUIRE(tags);

    CHECK(tags->getFrames().size() == 5);
    CHECK(tags->getStringValue(Tag::TITLE) == "Sample title");
    CHECK(tags->getStringValue(Tag::ARIST) == "画家");
    CHECK(tags->getStringValue(Tag::ALBUM) == "Sample album in UTF16-BE");
    CHECK(tags->getStringValue(Tag::TRACKNUMBER) == "3");
    CHECK(tags->getStringValue(Tag::DISCNUMBER) == "");
}

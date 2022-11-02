#include "data_builder.hpp"
#include "vector_reader.hpp"

#include <audiotag/file_reader.hpp>
#include <audiotag/mpeg/mpeg_file.hpp>
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
    constexpr auto title = "Sample title";
    constexpr auto artist = "Sample artist";
    constexpr auto album = "Sample album";
    constexpr auto year = "2022";
    constexpr auto comment = "Sample comment";
    constexpr auto track{ 255 };
    constexpr auto genre{ 250 };

    auto builder = DataBuilder{};
    builder.write(std::byte{ 0 }, 500);
    builder.write("TAG");
    builder.write(title);
    builder.write(std::byte{ 0 }, 30 - std::strlen(title));
    builder.write(artist);
    builder.write(std::byte{ 0 }, 30 - std::strlen(artist));
    builder.write(album);
    builder.write(std::byte{ 0 }, 30 - std::strlen(album));
    builder.write(year);
    builder.write(comment);
    builder.write(std::byte{ 0 }, 29 - std::strlen(comment));
    builder.write(std::byte{ track }, 1);
    builder.write(std::byte{ genre }, 1);

    const auto data = builder.build();

    auto reader = VectorReader{ data };
    MpegFile mpeg{ reader };

    REQUIRE_FALSE(mpeg.id3v2());

    const auto tags = mpeg.id3v1();
    REQUIRE(tags);

    CHECK(tags->title == title);
    CHECK(tags->artist == artist);
    CHECK(tags->album == album);
    CHECK(tags->year == year);
    CHECK(tags->comment == comment);
    CHECK(tags->track == track);
    CHECK(tags->genre == genre);
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

    CHECK_FALSE(mpeg.id3v1());

    const auto tags = mpeg.id3v2();
    REQUIRE(tags);

    CHECK(tags->getFrames().size() == 5);
    CHECK(tags->getStringValue(Tag::TITLE) == "Sample title");
    CHECK(tags->getStringValue(Tag::ARIST) == "画家");
    CHECK(tags->getStringValue(Tag::ALBUM) == "Sample album in UTF16-BE");
    CHECK(tags->getStringValue(Tag::TRACKNUMBER) == "3");
    CHECK(tags->getStringValue(Tag::DISCNUMBER) == "");
}

TEST_CASE("MpegFileWithInsufficientID3v1TagData")
{
    auto builder = DataBuilder{};
    builder.write("TAG");
    builder.write(std::byte{ 0 }, 10);

    const auto data = builder.build();

    auto reader = VectorReader{ data };
    MpegFile file{ reader };

    REQUIRE_FALSE(file.id3v1());
    REQUIRE_FALSE(file.id3v2());
}

TEST_CASE("MpegFileWithID3v10Comment")
{
    constexpr auto genre{ 25 };

    auto builder = DataBuilder{};
    builder.write("TAG");
    builder.write(std::byte{ 0 }, 94);
    builder.write(std::byte{ 0x61 }, 30);
    builder.write(std::byte{ genre }, 1);

    const auto data = builder.build();

    auto reader = VectorReader{ data };
    MpegFile file{ reader };

    CHECK_FALSE(file.id3v2());

    const auto tag = file.id3v1();
    REQUIRE(tag);
    CHECK(tag->comment == std::string(30, 'a'));
    CHECK(tag->track == 0);
    CHECK(tag->genre == genre);
}

TEST_CASE("MpegFileWithID3v11Comment")
{
    constexpr auto track{ 20 };
    constexpr auto genre{ 25 };

    auto builder = DataBuilder{};
    builder.write("TAG");
    builder.write(std::byte{ 0 }, 94);
    builder.write(std::byte{ 'a' }, 28); // comment
    builder.write(std::byte{ 0 }, 1); // null terminator
    builder.write(std::byte{ track }, 1);
    builder.write(std::byte{ genre }, 1);

    const auto data = builder.build();

    auto reader = VectorReader{ data };
    MpegFile file{ reader };

    CHECK_FALSE(file.id3v2());

    const auto tag = file.id3v1();
    REQUIRE(tag);
    CHECK(tag->comment == std::string(28, 'a'));
    CHECK(tag->track == track);
    CHECK(tag->genre == genre);
}

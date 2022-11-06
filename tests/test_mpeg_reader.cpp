#include "data_builder.hpp"
#include "id3v1_builder.hpp"
#include "id3v2_builder.hpp"
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
    const ID3v1::Tags expected{
        .title = "Sample title",
        .artist = "Sample artist",
        .album = "Sample album",
        .year = "2022",
        .comment = "Sample comment",
        .track = 255,
        .genre = 250,
    };

    auto builder = DataBuilder{};
    builder.write(std::byte{ 0 }, 500);
    builder.write(ID3v1Builder::build(expected));

    const auto data = builder.build();

    auto reader = VectorReader{ data };
    MpegFile mpeg{ reader };

    REQUIRE_FALSE(mpeg.id3v2());

    const auto tags = mpeg.id3v1();
    REQUIRE(tags);

    CHECK(tags->title == expected.title);
    CHECK(tags->artist == expected.artist);
    CHECK(tags->album == expected.album);
    CHECK(tags->year == expected.year);
    CHECK(tags->comment == expected.comment);
    CHECK(tags->track == expected.track);
    CHECK(tags->genre == expected.genre);
}

TEST_CASE("MpegFileWithID3v1Andv2Tags")
{
    FileReader reader{ TEST_DATA_DIR "/id3v2_id3v1.mp3" };
    MpegFile mpeg{ reader };

    REQUIRE(mpeg.id3v1());

    const auto id3v2 = mpeg.id3v2();
    REQUIRE(id3v2);

    CHECK(id3v2->getStringValue(Tag::TITLE) == "Sample title");
    CHECK(id3v2->getStringValue(Tag::ARIST) == "Sample artist in UTF16");
}

TEST_CASE("MpegFileWithID3v2TagsOnly")
{
    auto builder = DataBuilder{};
    builder.write(ID3v2::Identifier);
    builder.write(std::byte{ 2 }, 1); // version major
    builder.write(std::byte{ 3 }, 1); // version minor
    builder.write(std::byte{ 0 }, 1); // flags

    auto id3v2_builder = ID3v2Builder{};
    id3v2_builder.add_text_information_frame({ "TIT2", 0 }, "Sample title");
    id3v2_builder.add_text_information_frame({ "TPE1", 0 }, u"画家", 1, std::endian::big);
    id3v2_builder.add_text_information_frame({ "TALB", 0 }, u"アルバム", 2, std::endian::big);
    id3v2_builder.add_text_information_frame({ "TRCK", 0 }, u"5", 1, std::endian::little);

    auto id3v2_frames = id3v2_builder.build();
    builder.write_synch_safe(id3v2_frames.size());
    builder.write(id3v2_frames);

    const auto data = builder.build();

    auto reader = VectorReader{ data };
    MpegFile file{ reader };

    CHECK_FALSE(file.id3v1());

    const auto tags = file.id3v2();
    REQUIRE(tags);

    CHECK(tags->getFrames().size() == 4);
    CHECK(tags->getStringValue(Tag::TITLE) == "Sample title");
    CHECK(tags->getStringValue(Tag::ARIST) == "画家");
    CHECK(tags->getStringValue(Tag::ALBUM) == "アルバム");
    CHECK(tags->getStringValue(Tag::TRACKNUMBER) == "5");

    CHECK(tags->getStringValue(Tag::DISCNUMBER) == "");
}

TEST_CASE("MpegFileWithInsufficientID3v1TagData")
{
    auto builder = DataBuilder{};
    builder.write(ID3v1::Identifier);
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
    builder.write(ID3v1::Identifier);
    builder.write(std::byte{ 0 }, 94);
    builder.write(std::byte{ 'a' }, 30);
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
    builder.write(ID3v1::Identifier);
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

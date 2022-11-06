#pragma once

#include <cstdint>

namespace audiotag
{
enum class Encoding : std::uint8_t
{
    Latin1 = 0,
    UTF16 = 1,
    UTF16_BE = 2,
    UTF8 = 3,
};
}

#pragma once

#include <cstdint>

#if __cpp_lib_byteswap >= 202110L
#include <bit>
#endif

namespace audiotag
{
constexpr std::uint16_t byteswap(std::uint16_t value) noexcept
{
#if __cpp_lib_byteswap >= 202110L
    return std::byteswap(value);
#else // for now rely on gcc/clang builtins
    return __builtin_bswap16(value);
#endif
}

constexpr std::uint32_t byteswap(std::uint32_t value) noexcept
{
#if __cpp_lib_byteswap >= 202110L
    return std::byteswap(value);
#else // for now rely on gcc/clang builtins
    return __builtin_bswap32(value);
#endif
}
} // namespace audiotag

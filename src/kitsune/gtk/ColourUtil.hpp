#pragma once

#include "gdkmm/rgba.h"
#include <cstdint>
namespace kitsune::util {

inline constexpr GdkRGBA colour(int32_t hex) {
    return GdkRGBA {
        .red = ((hex & 0xff0000) >> 16) / 255.f,
        .green = ((hex & 0x00ff00) >> 8) / 255.f,
        .blue = ((hex & 0x0000ff) >> 0) / 255.f,
        .alpha = 1,
    };
}

inline constexpr int32_t reverse(const GdkRGBA& src) {
    return (int32_t(src.red * 255) << 16)
        + (int32_t(src.green * 255) << 8)
        + (int32_t(src.blue * 255) << 0);
}

}

#pragma once

#include "gdkmm/rgba.h"
#include <cstdint>
namespace kitsune::util {

inline consteval GdkRGBA colour(int32_t hex) {
    return GdkRGBA {
        .red = ((hex & 0xff0000) >> 16) / 255.f,
        .green = ((hex & 0x00ff00) >> 8) / 255.f,
        .blue = ((hex & 0x0000ff) >> 0) / 255.f,
        .alpha = 0,
    };
}

}

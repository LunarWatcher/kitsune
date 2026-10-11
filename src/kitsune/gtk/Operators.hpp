#pragma once

#include "gdkmm/rgba.h"

inline bool operator==(const GdkRGBA& left, const GdkRGBA& right) {
    return gdk_rgba_equal(&left, &right);
}

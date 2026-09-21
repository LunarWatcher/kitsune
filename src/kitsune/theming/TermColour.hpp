#pragma once

#include "gdk/gdk.h"
#include "kitsune/gtk/ColourUtil.hpp"
#include <string>
#include <vector>

namespace kitsune {

class TermColour {
private:
public:
    TermColour() = default;

    const GdkRGBA& foreground() const {
        static auto foreground = util::colour(0x000000);
        return foreground;
    }
    
    const GdkRGBA& background() const {
        static auto background = util::colour(0xffffff);
        return background;
    }

    const std::vector<GdkRGBA>& palette() const {
        // Export of my current konsole scheme (which itself is an export from a gnome terminal default scheme)
        static std::vector<GdkRGBA> palette = {
            util::colour(0x171421),
            util::colour(0xc01c28),
            util::colour(0x26a269),
            util::colour(0xa2734c),
            util::colour(0x12488b),
            util::colour(0xa347ba),
            util::colour(0x2aa1b3),
            util::colour(0xd0cfcc),
            util::colour(0x5e5c64),
            util::colour(0xf66151),
            util::colour(0x33d17a),
            util::colour(0xe9ad0c),
            util::colour(0x2a7bde),
            util::colour(0xc061cb),
            util::colour(0x33c7de),
            util::colour(0xffffff),
        };
        return palette;
    }

    const std::string& font() const {
        static std::string font = "SauceCodePRo Nerd Font 12";
        return font;
    }
};

}

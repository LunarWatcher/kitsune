#pragma once

#include "gdk/gdk.h"
#include "gdkmm/rgba.h"
#include "kitsune/gtk/ColourUtil.hpp"
#include <string>
#include <vector>

namespace kitsune {

/**
 * Global placeholder for the theme to hopefully make it easier to add more schemes or custom theme support in the
 * future if anyone else wants this functionality
 */
class TermColour {
private:
    // Export of my current konsole scheme (which itself is an export from a gnome terminal default scheme)
    std::vector<GdkRGBA> paletteData {
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
    GdkRGBA foregroundData = util::colour(0x000000);
    GdkRGBA backgroundData = util::colour(0xffffff);
    std::optional<std::string> fontStr;
public:
    TermColour() = default;

    const GdkRGBA& foreground() const {
        return foregroundData;
    }

    const GdkRGBA& background() const {
        return backgroundData;
    }

    const std::vector<GdkRGBA>& palette() const {
        return paletteData;
    }

    const std::optional<std::string>& font() const {
        return fontStr;
    }

    void replacePalette(
        GdkRGBA foreground,
        GdkRGBA background,
        std::vector<GdkRGBA>&& data
    ) {
        foregroundData = std::move(foreground);
        backgroundData = std::move(background);
        paletteData = std::move(data);
    }

    void setFont(std::string&& data) {
        fontStr = std::move(data);
    }
};

}

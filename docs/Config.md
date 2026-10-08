---
{
    "title": "Config"
}
---
# Config

## Files

The following two files are loaded, in order (provided they exist):

* `~/.config/kitsune/init.lua`
  * Intended for standard config. You can commit this file in a dotfile repo or similar.
* `~/.config/kitsune/local.lua`
  * Intended for system-local config. You should not commit this file.

## API

The full Lua API is available in config files. This section describes the API primarily meant for use in config files.

```lua
local config = require("kitsune.config");
```

### `config.setTermColours(colours: list<int>)`

Used to set the terminal colourscheme. The `colours` param is a list of 16 hex colours, corresponding to a standard 16 colour terminal scheme.

Example:
```lua
setTermColours({ 0x171421, 0xc01c28, ..., 0xffffff })
```

A default colourscheme based on gnome terminal's light mode scheme is provided if no scheme is set.

### `config.setTermFont(fontName: string)`

Sets the font to use in the terminal. This must be a valid font string for GTK, for example `SauceCodePro Nerd Font 11`

If no font is set, the system default monospace font is used.

Note that this font only affects the terminal, and not the rest of the GUI. At this time, no APIs are provided for setting the general GUI font. Consider using your system settings instead.

### `config.pipelineLookups(lookups: list<string>)`

Used to define pipeline lookup locations. `{{cwd}}/.kitsune` and `{{git_root}}/.kitsune` are always implicitly at the end of this list regardless of what you set it to here.


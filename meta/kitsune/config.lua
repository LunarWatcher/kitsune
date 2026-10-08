---@meta

local config = {}

--- Sets the terminal colourscheme
--- @param colours table<number> 16 hex values (0x000000-0xffffff) that make up the terminal colourscheme. Same setup as
---     other terminal emulators
function config.setTermColours(colours) end

--- Sets the font to use specifically in the terminal emulator
--- @param fontName string A font recognized by GTK, for example `SauceCodePro Nerd Font 11`
function config.setTermFont(fontName) end

--- Adds additional lookups to the default list of pipeline lookup locations.
--- @param extraLookups table<string>
function config.pipelineLookups(extraLookups) end

return config

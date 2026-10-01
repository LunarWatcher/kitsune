---
{
    "title": "Automation features"
}
---
# Automation features

The main reason kitsune exists is to provide terminal emulator-level automation capabilities by scripting terminals. This is somewhat inspired by zellij's ability to automate layouts, while not being intended as a terminal multiplexer.

In theory, kitsune could replace multiplexing as well, but this is a far future feature that isn't currently a priority. Its primary job is pipeline automation, parsing output from certain commands, and making it easier to define essentially sticky commands that rerun, and potentially retrigger other dependencies.

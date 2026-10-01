---
{
    "title": "Why a GUI?"
}
---
# Why a GUI?

GUIs just allow for more flexibility in the display itself. Though I would personally like a TUI, aside ncurses, there isn't really any good options for C++. The options are writing bindings to a rust library (I am never doing that shit again), or implementing one from scratch. For a standard TUI, that would be fine, but since this particular application includes embedding a full terminal, it's a fair bit more complicated.

Such a TUI likely needs to interpret the escape codes of its contained terminal in order for it to not escape visual containment and break the entire layout. This is still a problem I would like to solve at some point, but I still believe it to be the wrong solution for this particular tool.

Part of the problem to solve is automating terminal systems. This requires access to executing commands in a terminal environment, while being able to monitor its output. Kitsune includes (will include) functionality for parsing the output of certain commands to provide direct contextual information for commands that end up with too much output to be reasonably parsed manually, as well as providing direct execution that bypasses standard shells. Parts of this functionality is similar to zellij, but using a full GUI and significantly more automation and adaptive output parsing.

Why a GUI mostly comes down to the ability to more flexibly display and handle terminal content. Most of what I have described so far does not exclude a TUI (aside the implementation difficulty, for obvious reasons).

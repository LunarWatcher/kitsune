---
{
    "title": "Modal GUI"
}
---
# Modal GUI

Kitsune implements an elementary modal UI, i.e. a UI with modes. Since kitsune itself isn't an editor, the only three modes available are insert mode (for interaction with terminals), normal mode, and command mode.

This is largely implemented to allow more flexibility for inputs in TUIs. Specifically, it allows kitsune to disregard standard shortcuts used in TUIs or terminals in general, since they cannot conflict with anything used in the terminal (with one exception, as we'll see later).

## Major differences from modal editors

* Kitsune does not affect the behaviour within the terminal, so the terminal itself doesn't become modal. This is largely to avoid conflicts with embedded programs that are modal - like `emacs -nw` with evil mode, for example.
  * One consequence of this is that `i`, `a`, and `A`, some of the commands for entering insert mode, are all the same. Normal mode means interaction with the terminal buffer, aside switching focus of course, is entirely disabled
* Normal mode in kitsune is not an editor state as much as it is a buffer navigation and command state. If you want a vim-mode terminal, you'll need to set the corresponding settings for your shell, or use a plugin if none exist.
  * Zsh has a [built-in vim mode](https://stackoverflow.com/a/58188295)
* The shortcut for leaving insert mode (other editors: `ESC`) is `Alt-ESC`. This is to allow the use of standard modal editors within a terminal without causing conflicts.
  * This also means that, unlike normal terminal emulators, the only key you can conflict with is `Alt-ESC`. Interaction with kitsune is locked down in insert mode, and requires being in normal mode, so the other keybinds do not conflict with kitsune.


---
{
    "title": "Packaging notes"
}
---
# Packaging notes

## No `.desktop` files

Kitsune does not ship with `.desktop` files. Though kitsune is partly a GUI program, it's a GUI program that is explicitly meant to be spawned from the terminal. This is to allow dynamic working directories to be set per kitsune session, without requiring it be set manually via a GUI. Kitsune is closer to a CLI program that happens to pop up some windows.

Alternate package formats for kitsune should not ship `.desktop` files either.


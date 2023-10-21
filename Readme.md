[WIP] Clint
-----------

The C(wannabe)lint(er). Extract compiler diagnostics from any compiler with flags extracted from a compilation
database.

## Dependencies

- [Meson](https://github.com/mesonbuild/meson) build system.
- [Ninja](https://github.com/ninja-build/ninja) for building.
- [Json-c](https://github.com/json-c/json-c) for JSON parsing.
- [Unity](https://github.com/ThrowTheSwitch/Unity) for unit testing.
- A c compiler

## Installation

```
meson setup build
meson install -C build
```

## Running

You need a ```compile_commands.json``` in your project root. Run ```clint file.c``` and it should
output compiler warnings for the file.

## How it works

Clint searches the compilation database for the file, tokenises it (splits it into words)
and replaces the command output with /dev/null. It then runs the command and prints warnings
and errors.

## Acknowledges

Based on [gccdiag](https://gitlab.com/andrejr/gccdiag)

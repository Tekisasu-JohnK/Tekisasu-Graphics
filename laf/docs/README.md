# laf documentation

*laf* is a C++ library to create desktop applications for Windows,
macOS, and Linux. It was mainly developed for
[Aseprite](https://www.aseprite.org/) but we want to make it available
for other developers in such a way that they can create applications
too.

This "framework" (or library, or set of libraries) is still under
development, but we'd like to start stabilizing the main API in a near
future. The objective is to create a library as small as possible, and
leaving parts that are not required to be integrated, to other
libraries. For example, signal and slots can be used with [other
libraries](https://github.com/NoAvailableAlias/signal-slot-benchmarks),
or the clipboard management with [dacap/clip](https://github.com/dacap/clip),
etc. Even more, parts of *laf* might be separated to other libraries
in the future if they are highly valuable on their own.

## Modules

*laf* is a set of libraries:

* [base](base): Base functions for any kind of application (CLI and
  GUI), generally one function doesn't depend on others, so it has a
  minimal set of dependencies
* [gfx](gfx): Abstract graphics classes (rectangle, point, region,
  etc.)
* [ft](ft): FreeType wrapper used by the `os` module (requires
  `freetype` library as dependency). Might be deleted in the future
  (replaced with Skia text rendering)
* [os](os): Functions to create windows in your Operating System

## Platform-specific Definitions

Compiling in a specific operating system/platforms:

* `LAF_WINDOWS`
* `LAF_MACOS`
* `LAF_LINUX`

[*laf* backend](backend.md):

* `LAF_SKIA`: When we compile with `LAF_BACKEND=skia`, Skia library is available

CPU [endianness](https://en.wikipedia.org/wiki/Endianness) (defined in [base/config.h](https://github.com/aseprite/laf/blob/main/base/config.h.cmakein)):

* `LAF_LITTLE_ENDIAN`
* `LAF_BIG_ENDIAN`

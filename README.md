![JamEngine](https://i.imgur.com/eF0nArB.png)

Important Info
--------------
Right now JamEngine is only tested on Windows and Linux (and to be
honest I only test on Windows every couple months). Theoretically, JamEngine
should be compatible with any platform that SDL2 supports, but I can't
say for certain without testing it myself. The biggest thing is just
the functions `ns()` (Which the renderer uses) exclusively supports Windows,
Linux, and OSX; so that would have to be rewritten were you to use another
platform.

**Dependencies:** libxml2, sdl2, openal, alut

Features
========
 - Extremely easy API to use, all headers documented with doxygen (`$ doxygen doxyconfig.txt`)
 - Load many assets at once with an `JamAssetHandler`
 - Manage many entities at a time with a `JamWorld`
 - Between `JamWorld`s and `JamAssetHandler`s, memory management is quite simple as well
 - `JamBehaviourMap`s to automate most of each frame
 - Supports the use of [Tiled](https://www.mapeditor.org/) as a level editor
 - Written purely in C and thus cross platform
 - JamINI file I/O built in and extremely easy to use
 - Unicode text rendering (although bitmap fonts only at the moment)
 - Cross-platform implementations of atof and ftoa (string to double and vice-versa)
 - SAT collisions and simple rectangle/circle collisions are interchangeable
 - Collision grids (`JamTileMap`) for lightning-fast grid-to-rectangle collisions (or just rendering tiles (or both))
 - 3D audio support through OpenAL
 - Basic low-level tweening support for smooth animations 

To-Do List
==========
 - Input
    + Gamepad input
 - Drawing functions
    + Draw shapes
 - Fonts
    + TTF fonts
 - Audio
    + .ogg file loading
 - Renderer
    + Improve/make more consistent camera support
 - General
    + Make engine more resilient to bad values
![JamEngine](https://i.imgur.com/eF0nArB.png)

![Screenshot](https://i.ibb.co/9q7hrSX/screenshot.png)

Important Info
--------------
Right now JamEngine is only tested on Windows and Linux (and to be
honest I only test on Windows every couple months). Theoretically, JamEngine
should be compatible with any platform that SDL2 supports, but I can't
say for certain without testing it myself. The biggest thing is just
the functions `ns()` (Which the renderer uses) exclusively supports Windows,
Linux, and OSX; so that would have to be rewritten were you to use another
platform.

**Dependencies:** libxml2, sdl2, openal, alut, freetype, vulkan

Features
========
 - Extremely easy API to use, all headers documented with doxygen (`$ doxygen doxyconfig.txt`)
 - Load many assets at once with an `JamAssetHandler`
 - Manage many entities at a time with a `JamWorld`
 - Between `JamWorld`s and `JamAssetHandler`s, memory management is quite simple as well
 - `JamBehaviourMap`s to automate most of each frame
 - Supports the use of [Tiled](https://www.mapeditor.org/) as a level editor
 - Written purely in C
 - JamINI file I/O built in and extremely easy to use
 - Unicode text rendering, weather it be bitmap or true type
 - Cross-platform implementations of atof and ftoa (string to double and vice-versa)
 - SAT collisions and simple rectangle/circle collisions are interchangeable
 - TileMaps that can be used for rendering complex tile layers and checking collisions
 - 3D audio support through OpenAL
 - Optional input management system to abstract away the qualms of managing gamepads and different keys
 - World management system to make many levels easier
 - [Vulkan2D](https://github.com/PaoloMazzon/Vulkan2D) used to render, all of its power accessible inside the engine

Naming Conventions
==================
Since I'm starting to try and organize the names and make everything more consistent,
I'll set it in stone here.

 + Typedefs (and thus all JamEngine structs) all start with `Jam` and every word is capitalized
 + Functions begin with `jam` and every word after than is capitalized (`jSetError` is not an exception, its a macro. Its function counterpart is the exception)
 + Internally, variables names start with lowercase and every word after is uppercase (Java style)
 + Enum values start with their enum's abbreviation followed by an underscore (for example, `JamEntityType`'s things start with `et_*`)
 + Functions in the same file must all start with the same prefix (with some obvious exceptions, this is basically namespaces) (like how all functions in AssetHandler.h start with `jamAssetHandler*`) A rather large exception is no matter what file drawing functions belong to, they should be prefixed with `jamDraw*`

Usage
=====
This is still a bit of a work-in-progress, but should be at least a good starting point.

Windows
-------
Only msys2 is "officially" supported, as I do not have the time to maintain a Visual Studio project as well as
msys2/mingw64 which is my preferred way of developing on Windows. Once you have msys2 installed, how you compile
JamEngine is almost identical to compiling on Linux: use your package manager (pacman on msys2) to download the
dependencies listed at the top and then use cmake to make the makefile that will use gcc to compile an exe.
It should look something like this (from the MinGW64 shell, not the msys2 shell)

    pacman -S all-the-dependencies (use pacman -Ss to find specific package names)
    git clone https://github.com/PaoloMazzon/JamEngine.git
    cd JamEngine
    /mingw64/cmake.exe -G "MinGW Makefiles" CMakeLists.txt
    mingw32-make

Linux
-----
Using your preferred package manager, install the dependencies then use the cmake file. It will likely look
something like

    sudo apt-get install all-the-dependencies
    git clone https://github.com/PaoloMazzon/JamEngine.git
    cd JamEngine
    cmake CMakeLists.txt
    make 
 
Planned Future Additions
========================

In order of importance,

 1. Rework the tilemaps or make an additional version
 3. Use .json files instead of .ini files to load assets (as a way to ditch the prefix system)
 4. Make an IDE for JamEngine, most likely utilizing TCC and written using Kivy
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

**Dependencies:** libxml2, sdl2, openal, alut

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

Naming Conventions
==================
Since I'm starting to try and organize the names and make everything more consistent,
I'll set it in stone here.

 + Typedefs (and thus all JamEngine structs) all start with `Jam` and every word is capitalized
 + Functions begin with `jam` and every word after than is capitalized (`jSetError` is not an exception, its a macro. Its function counterpart is the exception)
 + Internally, variables names start with lowercase and every word after is uppercase (Java style)
 + Enum values start with their enum's abbreviation followed by an underscore (for example, `JamEntityType`'s things start with `et_*`)
 + Functions in the same file must all start with the same prefix (with some obvious exceptions, this is basically namespaces) (like how all functions in AssetHandler.h start with `jamAssetHandler*`) A rather large exception is no matter what file drawing functions belong to, they should be prefixed with `jamDraw*`

Future Additions
================
Near Future
-----------
Features that I've decided are vital to the engine itself and will be added
at some point in the near future. This would be things you simply expect
out of a game engine.

 - Better input support, including gamepad support and control maps
 - More control over audio (mainly pausing and related controls)
 - Entity pathing system (such as following predetermined paths and possibly A*)
 
Possible Additions
------------------
Various bits and pieces I think would be cool but aren't terribly important
to the JamEngine library itself and as such are prioritized much lower.

 - Make a neat little IDE-type interface so a beginner can focus on the code and less on the AssetHandler's files/behaviour maps
 - Some basic GUI type features in JamEngine like a textbox, buttons, drop down lists, etc...
 - Bind JamEngine to some languages like Lua and Python (and Lisp..! maybe)
 - Make rendering backend OpenGL instead of SDL2, but keep using SDL2 for window management and input
 - As an extension to the previous, GLSL shader support for fancy post processing 
 - Use .json files instead of .ini files to load assets (as a way to ditch the prefix system)
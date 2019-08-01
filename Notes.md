LJE (Light Jam Engine) uses SDL2
================================

Important Info
--------------
The struct definition for the Textures is held in the renderer
to avoid circular references, while the functions for textures
are stored in the Texture.h and Texture.c

This engine does not define any sort of audio functionality
because it would just be a very simple wrapper of SDL's
mixer. This is not needed because implementing SDL_mixer
audio is already extremely simple and would rather inefficient
to wrap every function. This may change in the future if I
decide to have the `AssetLoader` also handle audio but thats
a bit far off in the future.

Collision grids and tile maps are the same thing in this engine,
and an easy way to make use of this is to have a background
tile map for background/foilage/whatever and a foreground tile
map for trees/walls/rocks that is also what is used for collisons.
Just keep in mind that most of the time it is much cheaper to
have multiple tile maps over using entities as individual bits of
walls.

As far as this documentation is concerned, when it says "Exceptions" it
actually means that specific flag may be set to `jErrorCode` (which
can be checked via `jGetError`). If an error occurs, the program will
not cease to run, that's on you.

Asset Loading System
--------------------
The asset loader will load a single INI file at a time. This INI file is
expected to follow a certain format; every asset should have its own header
with a single-character prefix denoting the type, followed by an ID for the
asset. For example, a sprite header would look something like `[s481]`. You
could of course, also put a comment (with #) before every header to make it
clear what the asset is too.

By default, the prefixes are as follows:

 + `e` for entities
 + `s` for sprites
 + `m` for tile maps
 + `h` for hitboxes

Do note that textures are a special case. All textures should be denoted in
the INI by the following format:

    [texture_ids]
    43=path/to/image.png
    44=playersprite.png
    45=monster.png
    key=file
    ...

Where the key is id of the texture and file is the path to the texture in the
disk. *NOTE:* Do not use 0 as an ID as the loader needs that specific ID for
error checking.

Major To-Do List
----------------
 - Gamepad input
 - Rewrite the window resizing function so it doesn't crash the game if it fails
 - There is a potential memory leak in loadAssetIntoHandler where if you pass
  something with the same ID it will do nothing and create a memory leak with anything
  passed into it.
 - Make individual `load*` functions for asset handler that checks and makes sure the types
  match up (instead of doing this manually every time)
 - Load this into Windows and make sure everything works okay on MinGW or MSVC or whatever
 - Make a better error-checking system (in addition to `stderr`) that will likely be a global
  variable, a couple error setting/getting functions, and a bunch of error constants
 - Make drawSortedMap convert the `TileMap`'s values into sprite frames so it can be done
  quicker in the future.
 - Alot of Font.c is not verbose and this should be fixed.

Features
========
 - Renderer that handles all timing and rendering 
 - Asset Handler for loading and managing lots of things at once
 - Buffers that allow for quick binary access (they also handle loading from files)
 - Accurate cross-platform timing
 - Some basic drawing functionality
 - Entities, sprites, frames, hitboxes, and textures all different concepts. Different entities can have the same sprite; different sprites can use different regions of the same texture, etc...
 - Entity lists to make handling lots of entities easier
 - Sprites can be loaded from a sprite sheet with a single function
 - Worlds to make level management easier
 - Font framework that is quite flexible but only bitmap fonts right now
 - Font can do unicode, take varargs like printf (it accepts %f, %c, and %s)
 - Basic hitbox functionality
 - Incredibly easy-to-use INI file I/O as well as some other file I/O functionality
 - Pure-C (and thus cross-platform) implementations of atof and ftoa (string to double and double to string respectively)
 - Fair bit of string-manipulation functionality (see StringUtil.h)
 - Tile maps that can be auto tiled, used for collisions, used for enemy positions, etc...

Possible future additions
-------------------------
 - SAT Collisions
 - Vector images
 - Tweening

To-Do List
==========

 - Input System
    + Gamepad input
 - Renderer
    + Delta timing
 - Drawing functions
    + Let the fill colour thing work with alpha values
    + Draw shapes
    + Fonts (This implies that the font struct contains practical things only)
 - Hit-box
    + Circle-to-rectangle (Algorithm is written down just don't want to type it out)
 - Tile Maps
    + Auto tile to the grid (write down the sprite's frame as value)
 - Fonts
    + TTF fonts
 - Draw Queues/Events for multi-threading
    + Queue management
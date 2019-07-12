LJE (Light Jam Engine) uses SDL2
================================

IMPORTANT INFO
--------------
The struct definition for the Textures is held in the renderer
to avoid circular references, while the functions for textures
are stored in the Texture.h and Texture.c

This engine does not define any sort of audio functionality
because it would just be a very simple wrapper of SDL's
mixer. This is not needed because implementing SDL_mixer
audio is already extremely simple and would rather inefficient
to wrap every function.

Algorithm???: The world should be able to auto tile on the fly to
a texture that is of a size less than the world kind of in a chunk
system to only update and store in vram those on screen.

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
disk. 

Bits & Pieces To-Do
-------------------
 - Gamepad input
 - Rewrite the window resizing function so it doesn't crash the game if it fails

Possible future additions
-------------------------
 - SAT Collisions
 - Vector images
 - Tweening

World
 - Entity list of major entities
 - List of entity lists for things like NPCs and walls
 - Collision grids
 - Tile maps
 - Function that runs on creation, frame updates, and destruction

TODO (/ before description means done)
====================================
 - Textures
    + /Load textures
    + /Create from scratch
 - Input System
    + /Mouse input
    + /Keyboard input
    + Gamepad input
 - Renderer
    + Delta timing
    + /Create window
    + /Handle window scaling/fullscreen
    + /Variable size screen buffer and render size
    + /Main game loop
 - Drawing functions
    + /Render textures
    + Let the fill colour thing work with alpha values
    + Draw shapes
    + Fonts (This implies that the font struct contains practical things only)
 - Sprite
    + /Handle animations
    + /Frames
    + /Sprite rendering functions
    + /Load sprite sheet
 - Hit-box
    + /Circles
    + /Rectangles
    + Circle-to-rectangle (Algorithm is written down just don't want to type it out)
 - Entity
    + /Position in world
    + /Sprite
    + /Hit-box
    + /Speed/Friction
    + /Built-in processors
 - Tile Maps
    + /Store grid in memory
    + /Check collisions with entities
 - Fonts
    + /Bitmap fonts
    + /Unicode bitmap fonts
    + /Pseudo-printf-formatting (it accepts `%s` (string), `%c` (character), and `%f` (double))
    + TTF fonts
 - Draw Queues/Events for multi-threading
    + Queue management
 - Entity Lists - Important for handling lots of things
 - Particle System - Very notable addition
 - Worlds - Great feature yet not needed
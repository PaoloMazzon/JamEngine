![JamEngine](https://i.imgur.com/eF0nArB.png)

Important Info
--------------
The struct definition for the Textures is held in the renderer
to avoid circular references, while the functions for textures
are stored in the Texture.h and Texture.c

Right now JamEngine is only tested on Windows and Linux (and to be
honest I only test on Windows every couple months). Theoretically, JamEngine
should be compatible with any platform that SDL2 supports, but I can't
say for certain without testing it myself. The biggest thing is just
the functions `ns()` (Which the renderer uses) exclusively supports Windows,
Linux, and OSX; so that would have to be rewritten were you to use another
platform.

Worlds
------
Worlds are a tool that allow for you to have lots of entities and tilemaps on
screen at once without crazy headaches over managing the entities or memory.
Worlds have a couple key features

 + Entities are automatically sorted into two categories: type and range (although there is still a list of every entity)
 + filterEntitiesByProximity will automatically sort entities into in-range and out-of-range categories
 + Integrated `BehaviourMap` support (or more precisely `Behaviour` support)
 + Automatic memory management for all entities in the world
 + An array to store tile maps relevant to the world in

There are two important memory-related items to be aware of when using worlds, however.
Firstly, worlds are meant to be used in conjunction with an asset handler. As
such, worlds will not clean up the entitys' hitboxes, sprites, behaviour maps, etc...
This is because an asset handler will clean all of that up itself. As such, this
also means that under no circumstance can you add an entity from an asset handler
to a world directly; only add copies of entities from asset handlers to worlds. 
If you ignore this warning, you are near guaranteed segmentation faults when it comes
time to clean up the asset handler. This is what you may want adding entities to
a world to look like

    AssetHandler* handler = createAssetHandler();
    assetLoadINI(handler, renderer, "assets/mygame.ini", myBehaviourMap);
    worldAddEntity(gameWorld, copyEntity(assetGetEntity(handler, "EnemyEntity"), 80, 500));
    worldAddEntity(gameWorld, copyEntity(assetGetEntity(handler, "BirdEntity"), 200, 100));
    // Now my entity copies will be safely freed by the world, and the "real" entities loaded by
    // the asset handler (and its associated sprites/whatever) will be freed by the handler.

Second is that worlds do eat quite a bit of ram. An empty world is about 200 bytes, plus about
24 bytes per entity added (that's not counting how much the entity takes up or the alloc
cookies). You can expect a world with 1000 entities to eat up as much as 200kb of ram all
things considered (including entities and alloc cookies).

Major To-Do List
----------------
 - Gamepad input
 - Rewrite the window resizing function so it doesn't crash the game if it fails 
 - A lot of Font.c is not verbose and should be fixed.

Features
========
 - Extremely easy API to use, all headers documented with doxygen (`$ doxygen doxyconfig.txt`)
 - Load many assets at once with an `AssetHandler`
 - Manage many entities at a time with a `World`
 - Between `World`s and `AssetHandler`s, memory management is quite simple as well
 - `BehaviourMap`s to automate most of each frame
 - Supports the use of [Tiled](https://www.mapeditor.org/) as a level editor
 - Written purely in C and thus cross platform
 - INI file I/O built in and extremely easy to use
 - Unicode text rendering (although bitmap fonts only at the moment)
 - Cross-platform implementations of atof and ftoa (string to double and vice-versa)
 - SAT collisions and simple rectangle/circle collisions are interchangeable
 - Collision grids (`TileMap`) for lightning-fast grid-to-rectangle collisions (or just rendering tiles (or both))

Possible future additions (in no order)
---------------------------------------
 - Vector images
 - Tweening
 - Audio support through OpenAL

To-Do List
==========

 - Input System
    + Gamepad input
 - Renderer
    + Delta timing
 - Drawing functions
    + Let the fill colour thing work with alpha values
    + Draw shapes
 - Hit-box
    + Circle-to-polygon
 - Fonts
    + TTF fonts
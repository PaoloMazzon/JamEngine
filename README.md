![JamEngine](https://i.imgur.com/BRmHpKP.png)

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
not cease to run, that's on you. Please also note that while a function
may say it only throws x y or z, it may call other Jam Engine functions
that throw other things. This is to say that a function can indirectly
raise errors that weren't denoted in the root function's documentation.

Even though some assets may require other assets in the asset loader
(an entity needing a sprite and hitbox, for example), it does not matter
in what order they are defined in the INI, the asset loader will load
specifically in this order: textures -> sprites and hitboxes -> tilemaps
and entities. This way, no matter what assets rely on other assets, there
will never be an asset that is loaded before its dependencies.

Right now JamEngine is only tested on Windows and Linux (and to be
honest I only test on Windows every couple months). Theoretically, JamEngine
should be compatible with any platform that SDL2 supports, but I can't
say for certain without testing it myself. The biggest thing is just
the functions `ns()` (Which the renderer uses) exclusively supports Windows,
Linux, and OSX; so that would have to be rewritten were you to use another
platform.

Asset Loading System
--------------------
The asset loader will load a single INI file at a time. This INI file is
expected to follow a certain format; every asset should have its own header
with a single-character prefix denoting the type, followed by an ID for the
asset. For example, a sprite header would look something like `[sPlayerSprite]`. You
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
    imageTexture=path/to/image.png
    playerTexture=playersprite.png
    monsterTexture=monster.png
    key=file
    
    [sPlayerSprite]
    x_in_texture=0
    y_in_texture=0
    frame_width=16
    frame_height=16
    texture_id=playerTexture
    ...

Where the key is id of the texture and file is the path to the texture in the
disk. *NOTE:* Do not use 0 as an ID as the loader needs that specific ID for
error checking. Furthermore, it is vital that you never attempt to load an INI
more than once for each instance of `AssetLoader`. This is due to how it
manages memory (specifically the keys used as strings) and the fact that each
loader will hold on to its original INI until destruction. Having the old INI
available can make debugging easier and makes memory management a lot simpler
as well since the asset loader is just responsible for cleaning up the INI instead
of cleaning up * amount of strings. 

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

Behaviour Maps
--------------
Behaviour maps are a tool that is used to make large projects easier to manage.
You can make a behaviour map before you load with an asset manager and it will
automatically map the entities to their behaviours (a set of functions that are
called at different times like when its added to a world or drawn). Then, when
you use a world the world will automatically call whatever behaviour function
that needs to be called at that moment. The onCreate function is called when an
entity is added to a world through `worldAddEntity`, onDestroy is called when it
is removed from a world via `worldRemoveEntity`, onFrame is called once per frame,
and onDraw is called once per frame after all of the onFrame's have been called.
For any of these you just use the value NULL and nothing will be called (and no
segfaults will visit in your sleep). The parameters for any given `Behaviour`
function are as follows: `Renderer* renderer, World* world, Entity* self`. This
means every entity who's behaviour is called has immediate access to the world
its in, the renderer its being drawn to, and its own internal information.

So the general structure of these are as follows

 + You create a behaviour map and load in all of the behaviour functions
 + Then call assetLoadINI on your asset handler using your behaviour map
 + Now you may add copies of entities from the handler to your world where your world will handle the behaviours
 
This means most projects will require you to create 3 pointers when you game begins
and destroy 3 pointers once your game is over: an `AssetHandler`, `World`, and
`BehaviourMap`. Keep in mind that any pointers/memory allocated by one of those three
things will be cleaned up automatically when you free it; you don't have to manually
free anything but those three. 

As a side note, not defining an entity's behaviour when loaded from an asset handler will
try and give the entity a behaviour called "default". You can define a "default" behaviour
that will be given to any entities that haven't been given another behaviour. 

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
    + Circle-to-rectangle
 - Fonts
    + TTF fonts
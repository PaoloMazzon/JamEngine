LJE (Light Jam Engine)
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

Behaviour Maps
--------------
Behaviour maps are a tool that is used to make large projects easier to manage.
You can make a behaviour map before you load with an asset manager and it will
automatically map the entities to their behaviours (a set of functions that are
called at different times like when its added to a world or drawn). Then, when
you use a world the world will automatically call whatever behaviour function
that needs to be. For example

	BehaviourMap* map = createBehaviourMap();
	addBehaviourToMap(map, "Slime01", slimeCreated, slimeDies, NULL, slimeFrame, NULL, slimeDraw);
	addBehaviourToMap(map, "Player", playerCreated, playerDies, playerPreFrame, NULL, NULL, NULL);
	World* gameWorld = createWorld();
	// Add a bunch of entities - onCreation is called when you add the entity to a world

	...

	// Now you don't need a big update function, just call
	worldProcFrames(renderer, gameWorld);
	// And the preframe, frame, postframe, and draw functions will be called for each entity

As a side note, not defining an entity's behaviour when loaded from an asset handler will
try and give the entity a behaviour called "default". You can define a "default" behaviour
that will be given to any entities that haven't been given another behaviour. 

Major To-Do List
----------------
 - Gamepad input
 - Rewrite the window resizing function so it doesn't crash the game if it fails 
 - A lot of Font.c is not verbose and this should be fixed.

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
 - Worlds (or levels or rooms or whatever you want to call it) for level management
 - Worlds can automatically sort entities by range and type
 - Font framework that is quite flexible but only bitmap fonts right now
 - Font can do unicode, take varargs like printf (it accepts %f, %c, and %s)
 - Basic hitbox functionality
 - Incredibly easy-to-use INI file I/O as well as some other file I/O functionality
 - Pure-C (and thus cross-platform) implementations of atof and ftoa (string to double and double to string respectively)
 - Fair bit of string-manipulation functionality (see StringUtil.h)
 - Tile maps that can be auto tiled, used for collisions, used for enemy positions, etc...
 - Tested on both Windows and Linux

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
 - Hit-box
    + Circle-to-rectangle (Algorithm is written down just don't want to type it out)
 - Fonts
    + TTF fonts
JamAsset Handler & JamAsset Loading
=============================
To make loading many assets at once easier and more flexible,
JamEngine provides the `JamAssetHandler`. Each (individual) handler
can load a single .ini file containing all the data that handler
needs to load for whatever scene you're on. As of right now, 
handlers can load entities, textures, sprites, tilemaps,
hitboxes (but it cannot load the vertices on a polygon hitbox),
worlds, and audio buffers. The .ini files are expected to follow a 
strict format to be loaded, however.

This format is as follows

 + All textures must be a `key=val` pair of `id=path/to/file` under the header `[texture_ids]`
 + Each asset is under its own header of `[assetprefixASSET_ID]` followed by asset-type-specifc keys
 + Asset prefix are denoted beside their names below
 + Each asset has a set of keys the handler will recognize and parse, any other keys will be ignored (see below)
 + It does not matter what order you list the assets, the handler will load it properly
 + No asset is required to have any key, you can leave any field blank

And here is a list of recognized keys for every asset type

 + Entities ***(prefix = 'e')***
   + `sprite_id` ID of the sprite this entity uses (default=0 (NULL sprite pointer))
   + `hitbox_id` ID of the hitbox this entity uses (default=0 (NULL hitbox pointer))
   + `x` Default x coordinate (default=0)
   + `y` Default y coordinate (default=0)
   + `hitbox_offset_x` Default hitbox offset in the x direction (default=0)
   + `hitbox_offset_y` Default hitbox offset in the y direction (default=0)
   + `behaviour` The entity's behaviour to load from a `JamBehaviourMap` is the handler is given one (default="default")
   + `type` Type of entity this is (default="et_None") (types are Player, Item, Object, NPC, Solid, and Logic)
 + Sprites (the handler internally calls `jamLoadSpriteFromSheet` for every sprite) ***(prefix = 's')***
   + `texture_id` JamTexture to pull the sprite's frames from (default=0 (NULL pointer))
   + `animation_length` How many frames need to be loaded from the sheet (default=1)
   + `x_in_texture` X in the texture the spritesheet starts (default=0)
   + `y_in_texture` Y in the texture the spritesheet starts (default=0)
   + `frame_width` Width of each frame in the animation (default=16)
   + `frame_height` Height of each frame in the animation (default=16)
   + `padding_width` Horizontal space between each frame in texture (default=0)
   + `padding_height` Vertical space between each frame in texture (default=0)
   + `x_align` Horizontal offset of the animation in the spritesheet
   + `frame_delay` In-game frames between each animation frame
   + `looping` Weather or not the animation loops
 + Tile Maps ***(prefix = 'm')***
   + `file` The file to load the map from (default="") (see `jamLoadTileMap`)
   + `grid_width` How many tiles horizontally
   + `grid_height` How many tiles vertically
   + `cell_width` Width in pixels of each tile
   + `cell_height` Height in pixels of each tile
 + Hitboxes ***(prefix = 'h')***
   + `type` Type of hitbox (default="rectangle") (types are rectangle, circle, and polygon)
   + `width` Width of a rectangular hitbox (default=0)
   + `height` Height of a rectangular hitbox (default=0)
   + `radius` Radius of a circular hitbox (default=0)
 + Worlds ***(prefix = 'w')***
   + `file` The .tmx file to load the world from (The handler will call jamLoadWorldFromTMX internally)
 + Audio Buffers ***(prefix = 'a')***
   + `file` The file to load buffer from, as of right now only .wav files are supported

And an example ini file

    [texture_ids]
    imageTexture=path/to/image.png
    playerTexture=playersprite.png
    monsterTexture=monster.png
    key=file

    [ePlayer]
    sprite_id=PlayerSprite
    x=2130
    y=847
    behaviour=PlayerBehaviour
    
    [sPlayerSprite]
    x_in_texture=0
    y_in_texture=0
    frame_width=16
    frame_height=16
    texture_id=playerTexture
    ...

Notice how the entity Player is listed before the PlayerSprite even
though it depends on PlayerSprite. This doesn't matter, the handler
will still load it properly

Behaviours are covered in another document.

Warning
-------
Don't use 0 as an id, the handler needs it for handling NULL pointers
and default values. Also, asset names exclude their prefix. For example
an asset might have the header `[sPlayerSprite]` in the .ini file, but
its actual asset name is `PlayerSprite`.

Also, always free behaviour maps LAST. They depend on nothing but several
things depend on them.
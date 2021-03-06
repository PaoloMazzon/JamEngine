Worlds & Level Loading
======================
Worlds are a tool that allow for you to have lots of entities and tilemaps on
screen at once without crazy headaches over managing the entities or memory.
Worlds have a couple key features

 + Entities are automatically sorted into two categories: type and range (although there is still a list of every entity)
 + jamWorldFilter will automatically sort entities into in-range and out-of-range categories
 + Integrated `JamBehaviourMap` support (or more precisely `JamBehaviour` support)
 + Automatic memory management for all entities in the world
 + An array to store tile maps relevant to the world in

There are two important memory-related items to be aware of when using worlds, however.
Firstly, worlds are meant to be used in conjunction with an asset handler. As
such, worlds will not clean up the entitys' hitboxes, sprites, behaviour maps, etc...
This is because an asset handler will clean all of that up itself (save for behaviour maps).
As such, this also means that under no circumstance can you add an entity from an asset handler
to a world directly; only add copies of entities from asset handlers to worlds. 
If you ignore this warning, you are near guaranteed segmentation faults when it comes
time to clean up the asset handler. This is what you may want adding entities to
a world to look like

    JamAssetHandler* handler = jamAssetHandlerCreate();
    jamAssetHandlerLoadINI(handler, renderer, "assets/mygame.ini", myBehaviourMap);
    jamWorldAddEntity(gameWorld, jamEntityCopy(jamAssetHandlerGetEntity(handler, "EnemyEntity"), 80, 500));
    jamWorldAddEntity(gameWorld, jamEntityCopy(jamAssetHandlerGetEntity(handler, "BirdEntity"), 200, 100));
    // Now my entity copies will be safely freed by the world, and the "real" entities loaded by
    // the asset handler (and its associated sprites/whatever) will be freed by the handler.

That would, however, take quite a bit of time to write out each entity by hand like that so
instead you can just load worlds from .tmx files using `jamTMXLoadWorld` Using Tiled to
create worlds can make the level-building process much quicker, but using Tiled comes
with a few caveats to be aware of due to the limitations of JamEngine

 + Tileset layers can be whatever, but the tilesets must be named after the sprite they come from in the handler
 + All entities in the world must be in an object layer, but how many object layers you have is irrelevant
 + Each entity (or object in Tiled) must have its type be the same as its asset name
 + Only orthogonal worlds are supported, sorry isometric fans

Tiled might take a bit of fiddling to get working properly for you, but it is very
handy once it is.

Note For .tmx Loading
---------------------

JamAssetHandler supports the loading of .tmx files directly through the handler. For convenience
you can just add it to your assets' .ini file

    [wGrassLevel]
    file=level1.tmx
    
    [wUnderwaterLevel]
    file=level2.tmx

Loading from .tmx files automatically sets the spatial map width and height to double the tile
width and height.
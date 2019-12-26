Sprites, Animations, & Tweening
===============================
JamEngine supports a few thing to make animations easier such as sprites and
tweening support. Sprites will likely be essential to most projects you make
and as such, they are very versatile in their implementation. 

Sprites
-------
Sprites are at their core quite simple. They are essentially just fancy arrays
of frames, and frames are themselves just pointers to textures alongside a
rectangle denoting the part in the texture to draw. This abstraction, however,
makes how you load and render sprites extremely versatile; you can have an
animation that comes from an infinite number of different textures, several
textures each with a couple frames a piece, or most commonly many frames using
only a single texture with different regions for each frame. You could sprites
for anything from rendering a character's many-frame animation to a tileset used
to render the game world (as is the case with `jamTileMapAuto`).

Other than manually setting up the arrays in the sprite, you have three JamEngine
things to help you with loading sprites

 + `jamSpriteAppendFrame` will append a frame to a sprite (see `jamFrameCreate`)
 + `jamSpriteLoadFromSheet` is used to load an animation from a single texture
 + Use a JamAssetHandler (although it only supports `jamSpriteLoadFromSheet` for loading, but streamlines the process)

Tweening
--------
Tweening in JamEngine is a tool used to make the transition between two numerical
points smoother. Typically, you would use this for something like a UI element or
some sort of entity movement. For an example, compile the demo and run it. The menu
uses `JamTweenState`s for the buttons.
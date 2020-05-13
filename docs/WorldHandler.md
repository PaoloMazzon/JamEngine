World Handler
=============

JamEngine provides a way to manage multiple levels at once through
the world handler. The world handler holds onto information about
worlds in its internal array and loads/unloads .tmx worlds on the
fly as to provide a simple way to have some sort of level system for
your games. You can change worlds on the fly, reset them, or even
not use a world at all (for example, a menu might just be a function
that changes the current world).

The world handler is also (as JamEngine tries to be) very simple to
use. The renderer automatically initializes and cleans up the world.
Even if you choose not to use it, its not a concern because without
any loaded worlds the memory overhead is extremely small (around ~20 
bytes, and the performance overhead is zero until the renderer is 
finally cleaned up, at which point its 1 function call). First, you
would load the levels into the handler

    jamWorldHandlerAdd("tmx/mylevel1.tmx, NULL, onWorldCreate, onWorldFrame, onWorldCleanup);
    jamWorldHandlerAdd("tmx/mylevel2.tmx, NULL, onWorldCreate, onWorldFrame, onWorldCleanup);
    jamWorldHandlerAdd("tmx/mylevel3.tmx, NULL, onWorldCreate, onWorldFrame, onWorldCleanup);
    
The name of the worlds are used when you want to switch worlds. You
can (as you see above) use NULL as the name, which makes the world
automatically use the filename as the world's name (Without the
extension or path, so the first name above would be "mylevel1"). You
may choose to specify functions to be called when the world is loaded,
ran each frame, and just before its cleaned up, or NULL. If you choose
to not provide a function for the onFrame argument, the world handler
will automatically call jamWorldProcFrame for the world.

Should you want to switch worlds or quit, you can use the functions
`jamWorldHandlerSwitch`, `jamWorldHandlerNext`, and `jamWorldHandlerRestart`.
If you use the macro `WORLD_QUIT` for `jamWorldHandlerSwitch`, it
will quit running the world handler, or of course you could switch
to the named level. Since the world handler uses a sequential array
to store world data, you can use `jamWorldHanderNext` to switch to
the next level (or go from the last level to the first), and obviously
`jamWorldHandlerRestart` is used to reload the current level.

Once you're done loading levels into the handler, you simply call
`jamWorldHandlerRun` and it will begin processing the game. It
handles the main game loop itself safely and as such will stop and
cleanup should a jam error be raised.
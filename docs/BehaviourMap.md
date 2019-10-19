Behaviour Maps & Automated Entity Function Calls
================================================
Behaviour maps are a way of automating an entity's behaviour. They make
managing many entities at once much easier when used with a world. The
general form of using behaviour maps is as follows:

 + Create a behaviour map and load it with behaviours
 + Create & load an asset handler, and the handler will assign the behaviours to the entities it creates
 + When you put all of these entities into a world, the world will call their corresponding behaviours when appropriate

An individual behaviour in a map consists of 4 function pointers: onCreate, onDestroy, onFrame,
onDraw. As you might guess, onCreate and onDestroy are called when an entity is added to a
world and removed respectively. onFrame is called each frame by the world when you call `worldProcFrame`
is called, and onDraw is a bit more tricky. If onDraw is a null pointer, the world will just draw
the entity as normal, but if onDraw is not null, the world will not draw the entity at all and that
responsibility then falls to the function. This allows you to render certain entities differently
than other without wasting time rendering the entity via `drawEntity`.

The function pointers that behaviour maps expect should be as follows

    void functionName(JamRenderer*, World*, Entity*);

This is true for all 4 different functions. The renderer is passed to every entity, the world
that called the function is passed, and the entity who's being processed gets access to its
own pointer as well. 

As a simple example,

    // Create a behaviour map and put our behaviours into it
    BehaviourMap* bMap = createBehaviourMap();
	addBehaviourToMap(bMap, "PlayerBehaviour", onPlayerCreate, onPlayerDestroy, onPlayerFrame, NULL);
	addBehaviourToMap(bMap, "EnemyBehaviour", onEnemyCreate, onEnemyDestroy, onEnemyFrame, onEnemyDraw);
	
	// Create and then load our asset handler, passing the behaviour map to jamAssetLoadINI
	JamAssetHandler* handler = jamCreateAssetHandler();
	jamAssetLoadINI(handler, renderer, "assets/level0.ini", bMap);
	
	// Now we load a world from a tmx file and when we do, all of the assets' onCreate will be called
	World* gameWorld = loadWorldFromTMX(handler, renderer, "assets/level0.tmx");

TMX loading is covered in the World documentation.
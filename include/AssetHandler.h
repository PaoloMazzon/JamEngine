/// \file AssetHandler.h
/// \author lugi1
/// \brief Makes loading lots of things easier
#pragma once

#include "Sprite.h"
#include "Texture.h"
#include "TileMap.h"
#include "Font.h"
#include "Entity.h"
#include "Hitbox.h"
#include "INI.h"
#include "BehaviourMap.h"

enum JamAssetType {at_Texture, at_Sprite, at_TileMap, at_Entity, at_Hitbox};

/// \brief A struct that can hold any of the assets the asset handler needs
///
/// This struct utilizes an anonymous union inside it so it can hold
/// any of the needed assets without using a bunch of extra storage.
typedef struct {
	enum JamAssetType type; ///< The type of asset this thing holds

	union {
		JamTexture* tex; ///< The internal texture
		JamSprite* spr; ///< The internal sprite
		JamTileMap* tileMap; ///< The internal tile map
		JamEntity* entity; ///< The internal entity
		JamHitbox* hitbox; ///< The internal hitbox
	};
} JamAsset;

/// \brief In case you wish to change what type of keys are used
typedef const char* JamAssetKey;

/// \brief Loads lots of assets at once from files
/// to manage large projects
///
/// Do not use these lightly, they are an incredibly
/// helpful tool but they gobble up memory like there
/// is no tomorrow.
///
/// Also, something incredibly important to note is that
/// this class expects all strings (from the key array)
/// to be dynamically allocated since they will all be
/// later freed upon destruction. Do not manually set
/// keys.
///
/// Because it is important in which order assets are
/// loaded, this handler only lets you load directories.
/// The recognized file formats are .ent, .spr, .png,
/// .hit, .level, and .bmp. The loader always loads
/// in the order Textures -> Sprites -> Entities.
/// Where hitboxes/tile maps fit in there is not at
/// important since they don't depend on other assets
/// being loaded in first.
typedef struct {
	int size; ///< The size of the map
	JamAssetKey* ids; ///< The keys that match up with the values
	JamAsset** vals; ///< The actual assets
	JamINI* localINI; ///< Let the INI keep track of internal string's memory
} JamAssetHandler;

/// \brief Creates an asset handler
/// \throws ERROR_ALLOC_FAILED
JamAssetHandler* jamCreateAssetHandler();

/// \brief Throws an asset into the handler
///
/// Once an asset is thrown in here, the handler will take full
/// responsibilty for its cleanup except for two cases: an entity's
/// sprite and hitbox (Those must be loaded independently). Do
/// not clean up assets yourself if you throw them into a handler.
///
/// \warning Never run this more than once per assetLoader object, otherwise
/// you will more than likely get a segfault and if not at least a memory leak.
/// \throws ERROR_REALLOC_FAILED
/// \throws ERROR_NUULL_POINTER
void jamLoadAssetIntoHandler(JamAssetHandler *handler, JamAsset *asset, JamAssetKey id);

/// \brief Loads all recognized assets from a directory
///
/// Loads all assets from an INI file. Note that you should never use 0
/// as an ID for an asset or the loader may not load everything correctly.
/// If you don't have/want a behaviour map you can just pass NULL.
///
/// \throws ERROR_ASSET_NOT_FOUND
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_OPEN_FAILED
void jamAssetLoadINI(JamAssetHandler *assetHandler, JamRenderer *renderer, const char *filename, JamBehaviourMap *map);

/// \brief Grabs an asset, or returns NULL if the key is not bound
/// \throws ERROR_NULL_POINTER
JamAsset* jamGetAssetFromHandler(JamAssetHandler *assetHandler, JamAssetKey key);

/// \brief Pulls a specific asset safely, making sure the types match up
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ASSET_WRONG_TYPE
JamSprite* jamGetSpriteFromHandler(JamAssetHandler *handler, JamAssetKey key);

/// \brief Pulls a specific asset safely, making sure the types match up
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ASSET_WRONG_TYPE
JamEntity* jamGetEntityFromHandler(JamAssetHandler *handler, JamAssetKey key);

/// \brief Pulls a specific asset safely, making sure the types match up
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ASSET_WRONG_TYPE
JamHitbox* jamGetHitboxFromHandler(JamAssetHandler *handler, JamAssetKey key);

/// \brief Pulls a specific asset safely, making sure the types match up
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ASSET_WRONG_TYPE
JamTexture* jamGetTextureFromHandler(JamAssetHandler *handler, JamAssetKey key);

/// \brief Pulls a specific asset safely, making sure the types match up
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ASSET_WRONG_TYPE
JamTileMap* jamGetTileMapFromHandler(JamAssetHandler *handler, JamAssetKey key);

/// \brief Frees an asset handler and all of its components
JamAssetHandler* jamFreeAssetHandler(JamAssetHandler *handler);
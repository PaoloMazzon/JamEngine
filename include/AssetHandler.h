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

enum AssetType {texAsset, sprAsset, tileAsset, entAsset, hitAsset};

/// \brief A struct that can hold any of the assets the asset handler needs
///
/// This struct utilizes an anonymous union inside it so it can hold
/// any of the needed assets without using a bunch of extra storage.
typedef struct {
	enum AssetType type; ///< The type of asset this thing holds

	union {
		Texture* tex; ///< The internal texture
		Sprite* spr; ///< The internal sprite
		TileMap* tileMap; ///< The internal tile map
		Entity* entity; ///< The internal entity
		Hitbox* hitbox; ///< The internal hitbox
	};
} Asset;

typedef const char* AssetKey;

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
	AssetKey* ids; ///< The keys that match up with the values
	Asset** vals; ///< The actual assets
	INI* localINI; ///< Let the INI keep track of internal string's memory
} AssetHandler;

/// \brief Creates an asset handler
/// \throws ERROR_ALLOC_FAILED
AssetHandler* createAssetHandler();

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
void loadAssetIntoHandler(AssetHandler* handler, Asset* asset, AssetKey id);

/// \brief Loads all recognized assets from a directory
///
/// Loads all assets from an INI file. Note that you should never use 0
/// as an ID for an asset or the loader may not load everything correctly.
/// If you don't have/want a behaviour map you can just pass NULL.
///
/// \throws ERROR_ASSET_NOT_FOUND
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_OPEN_FAILED
void assetLoadINI(AssetHandler* assetHandler, Renderer* renderer, const char* filename, BehaviourMap* map);

/// \brief Grabs an asset, or returns NULL if the key is not bound
/// \throws ERROR_NULL_POINTER
Asset* assetGet(AssetHandler* assetHandler, AssetKey key);

/// \brief Pulls a specific asset safely, making sure the types match up
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ASSET_WRONG_TYPE
Sprite* assetGetSprite(AssetHandler* handler, AssetKey key);

/// \brief Pulls a specific asset safely, making sure the types match up
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ASSET_WRONG_TYPE
Entity* assetGetEntity(AssetHandler* handler, AssetKey key);

/// \brief Pulls a specific asset safely, making sure the types match up
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ASSET_WRONG_TYPE
Hitbox* assetGetHitbox(AssetHandler* handler, AssetKey key);

/// \brief Pulls a specific asset safely, making sure the types match up
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ASSET_WRONG_TYPE
Texture* assetGetTexture(AssetHandler* handler, AssetKey key);

/// \brief Pulls a specific asset safely, making sure the types match up
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ASSET_WRONG_TYPE
TileMap* assetGetTileMap(AssetHandler* handler, AssetKey key);

/// \brief Frees an asset handler and all of its components
AssetHandler* freeAssetHandler(AssetHandler* handler);
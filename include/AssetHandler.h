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
	int* ids; ///< The keys that match up with the values
	Asset** vals; ///< The actual assets
} AssetHandler;

/// \brief Creates an asset handler
AssetHandler* createAssetHandler();

/// \brief Throws an asset into the handler
///
/// Once an asset is thrown in here, the handler will take full
/// responsibilty for its cleanup except for two cases: an entity's
/// sprite and hitbox (Those must be loaded independently). Do
/// not clean up assets yourself if you throw them into a handler.
void loadAssetIntoHandler(AssetHandler* handler, Asset* asset, int id);

/// \brief Loads all recognized assets from a directory
///
/// Loads all assets from an INI file.
void assetLoadINI(AssetHandler* assetHandler, Renderer* renderer, const char* filename);

/// \brief Makes sure the assets in given ranges match their proper type
///
/// This function checks to make sure that every number between each range
/// is its respective asset type. This is useful for mass checking what
/// you load, since you can set it up such that all sprites are between
/// 1000-1999 and so on. This function also makes sure that all the assets
/// are not NULL pointers. All problems reported to stderr.
bool assetAssertRanges(AssetHandler* handler, int entRangeStart, int entRangeEnd, int sprRangeStart, int sprRangeEnd,
					   int tileRangeStart, int tileRangeEnd, int texRangeStart, int texRangeEnd, int hitRangeStart, int hitRangeEnd);

/// \brief Grabs an asset, or returns NULL if the key is not bound
Asset* assetGet(AssetHandler* assetHandler, int key);

/// \brief Frees an asset handler and all of its components
AssetHandler* freeAssetHandler(AssetHandler* handler);
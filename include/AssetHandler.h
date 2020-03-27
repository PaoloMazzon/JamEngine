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
#include "Audio.h"
#include "World.h"
#include "Font.h"

#ifdef __cplusplus
extern "C" {
#endif

enum JamAssetType {at_Texture, at_Sprite, at_Entity, at_Hitbox, at_AudioBuffer, at_World, at_BFont, at_Font};

/// \brief A struct that can hold any of the assets the asset handler needs
typedef struct __sJamAsset JamAsset;
struct __sJamAsset{
	enum JamAssetType type; ///< The type of asset this thing holds
	union {
		JamTexture* tex;        ///< The internal texture
		JamSprite* spr;         ///< The internal sprite
		JamEntity* entity;      ///< The internal entity
		JamHitbox* hitbox;      ///< The internal hitbox
		JamAudioBuffer* buffer; ///< The internal audio buffer
		JamWorld* world;        ///< The internal world
		JamBitmapFont* bitFont; ///< The internal bitmap font
		JamFont* font;          ///< The internal font
	};
	char* name;     ///< Name of the asset for hashing purposes
	JamAsset* next; ///< In the case of hashing collisions
};

/// \brief Loads lots of assets at once from files
/// to manage large projects
///
/// Asset handlers make loading large amounts of resources at once very
/// simple and easy. That said, they are incredibly heavy on memory because
/// they utilize a hash table internally so lookup time for assets isn't
/// crazy.
typedef struct {
	int size;        ///< The size of the vals array
	JamAsset** vals; ///< The actual assets
} JamAssetHandler;

/// \brief Creates an asset handler
///
/// The size parameter is how many elements to allocate in the hash
/// bucket, bigger values means more memory taken but better performance,
/// smaller values means less memory taken but potentially much slower.
/// If you have a lot of assets in your game, you may want a much larger
/// value and vice versa. For most things, 1000 should be a happy medium.
///
/// Please note that size is not the maximum number of elements the handler
/// can hold, just the hash table bucket size. You can technically set it to
/// 1 but that would be extremely slow.
///
/// \throws ERROR_ALLOC_FAILED
JamAssetHandler* jamAssetHandlerCreate(int size);

/// \brief Throws an asset into the handler
///
/// Once an asset is thrown in here, the handler will take full
/// responsibility for its cleanup except for two cases: an entity's
/// sprite and hitbox (Those must be loaded independently). Do
/// not clean up assets yourself if you throw them into a handler.
///
/// This function will replace the asset if there is already one there
/// and post a warning to jSetError
///
/// \throws ERROR_REALLOC_FAILED
/// \throws ERROR_NUULL_POINTER
void jamAssetHandlerLoadAsset(JamAssetHandler *handler, JamAsset *asset, const char *id);

/// \brief Loads all recognized assets from a directory
///
/// Loads all assets from an INI file. Note that you should never use 0
/// as an ID for an asset or the loader may not load everything correctly.
/// If you don't have/want a behaviour map you can just pass NULL.
///
/// You can load as many inis into a single handler as you want.
///
/// \throws ERROR_ASSET_NOT_FOUND
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_OPEN_FAILED
void jamAssetHandlerLoadINI(JamAssetHandler *assetHandler, const char *filename, JamBehaviourMap *map);

/// \brief Pulls a specific asset safely, making sure the types match up
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ASSET_WRONG_TYPE
JamSprite* jamAssetHandlerGetSprite(JamAssetHandler *handler, const char *key);

/// \brief Pulls a specific asset safely, making sure the types match up
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ASSET_WRONG_TYPE
JamEntity* jamAssetHandlerGetEntity(JamAssetHandler *handler, const char *key);

/// \brief Pulls a specific asset safely, making sure the types match up
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ASSET_WRONG_TYPE
JamHitbox* jamAssetHandlerGetHitbox(JamAssetHandler *handler, const char *key);

/// \brief Pulls a specific asset safely, making sure the types match up
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ASSET_WRONG_TYPE
JamTexture* jamAssetHandlerGetTexture(JamAssetHandler *handler, const char *key);

/// \brief Pulls a specific asset safely, making sure the types match up
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ASSET_WRONG_TYPE
JamAudioBuffer* jamAssetHandlerGetAudioBuffer(JamAssetHandler *handler, const char *key);

/// \brief Pulls a specific asset safely, making sure the types match up
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ASSET_WRONG_TYPE
JamWorld* jamAssetHandlerGetWorld(JamAssetHandler *handler, const char *key);

/// \brief Pulls a specific asset safely, making sure the types match up
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ASSET_WRONG_TYPE
JamFont* jamAssetHandlerGetFont(JamAssetHandler *handler, const char *key);

/// \brief Pulls a specific asset safely, making sure the types match up
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ASSET_WRONG_TYPE
JamBitmapFont* jamAssetHandlerGetBitmapFont(JamAssetHandler *handler, const char *key);

/// \brief Frees an asset handler and all of its components
void jamAssetHandlerFree(JamAssetHandler *handler);

#ifdef __cplusplus
}
#endif
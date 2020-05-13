/// \file TMXWorldLoader.h
/// \author plo
/// \brief Loads a world from an asset handler and tmx file

#pragma once
#include "World.h"
#include "AssetHandler.h"
#include "Renderer.h"

struct _JamColour;

#ifdef __cplusplus
extern "C" {
#endif

/// \brief The types of properties accessible in the JamTMXData
typedef enum {tt_Bool, tt_String, tt_Int, tt_Float, tt_Colour, tt_File} JamTMXType;

/// \brief A singular property in a JamTMXData struct
typedef struct {
	JamTMXType type;           ///< What type of property this is
	union {
		bool boolVal;                ///< Boolean value, if applicable
		const char* stringVal;       ///< String value, if applicable
		int intVal;                  ///< Integer value, if applicable
		float floatVal;              ///< Float value, if applicable
		struct _JamColour colourVal; ///< Colour value, if applicable (converted to a JamColour struct)
		const char* fileVal;         ///< File value, if applicable (which is just a c-string)
	};
} JamTMXProperty;

/// \brief Holds information a tmx file will provide to entities
typedef struct _JamTMXData {
	JamTMXProperty** values; ///< The values themselves
	const char** names;      ///< The names of all the properties
	int size;                ///< Number of elements in this struct
} JamTMXData;

/// \brief Creates a JamTMXData
/// \throws ERROR_ALLOC_FAILED
JamTMXData* jamTMXDataCreate();

/// \brief Gets a property from a tmx data
///
/// Unlike other Jam functions, if the given pointer is NULL, this
/// function will not throw an error since not every entity is expected
/// to have properties from the tmx file.
JamTMXProperty* jamTMXDataGetProperty(JamTMXData* data, const char* propertyName);

/// \brief Frees a tmx data
///
/// Entities call this automatically, you don't need to
void jamTMXDataFree(JamTMXData* data);

/// \brief Loads a world from a tmx level file
/// \param handler The handler that already has its assets loaded 
/// \param tmxFilename The filename of the tmx file to load
///
/// Refer to the World documentation for how this function behaves.
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_OPEN_FAILED
/// \throws ERROR_ALLOC_FAILED
JamWorld* jamTMXLoadWorld(JamAssetHandler *handler, const char *tmxFilename);

#ifdef __cplusplus
}
#endif
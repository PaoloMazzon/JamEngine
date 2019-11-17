/// \file StringMap.h
/// \author lugi1
/// \brief Declares a string-map system for C-style strings
///
/// This should not be used in real time, rather for things
/// like loading and other operations not done on a frame-
/// to-frame basis.

#pragma once
#include "stdio.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief A key/val map for string:string operations
typedef struct {
	int size; ///< Size of the two lists in elements
	int sizeOfGarbagePile; ///< Pretty self-explanatory
	char** keys; ///< The keys
	char** vals; ///< The values
	char** garbage; ///< Heap-based strings that need to be freed upon destruction
} JamStringMap;

/// \brief Creates an empty map
/// \throws ERROR_ALLOC_FAILED
JamStringMap* jamCreateStringMap();

/// \brief Loads a map from an smap file
/// \throws ERROR_OPEN_FAILED
/// \throws ERROR_ALLOC_FAILED
/// \throws ERROR_INCORRECT_FORMAT
JamStringMap* jamLoadStringMap(const char *filename);

/// \brief Prints the map in INI format to the given stream
/// \throws ERROR_NULL_POINTER
void jamOutputStringMap(JamStringMap *smap, FILE *stream);

/// \brief Sets a value to a key in a map
/// \throws ERROR_NULL_POINTER
void jamSetStringMapVal(JamStringMap *smap, const char *key, char *val);

/// \brief Gets a value from a map
/// \throws ERROR_NULL_POINTER
const char* jamGetStringMapVal(JamStringMap *smap, const char *key, char *def);

/// \brief Throws a string into the garbage
///
/// Do this for strings you're using in this thing that are dynamically
/// allocated so freeSMap can take care of them.
///
/// \throws ERROR_REALLOC_FAILED
/// \throws ERROR_NULL_POINTER
void jamThrowInGarbageStringMap(JamStringMap *smap, char *garbage);

/// \brief Frees the map
void jamFreeStringMap(JamStringMap *smap);

#ifdef __cplusplus
}
#endif
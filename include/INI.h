/// \file INI.h
/// \author lugi1
/// \brief An easy way to manage INI files
///
/// This file makes it easy to create load and manage
/// INI files in C.

#pragma once
#include "StringMap.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Holds information pertaining to INI files
typedef struct {
	JamStringMap** headers; ///< Each one is a different header
	char** headerNames; ///< Corrosponding name of each header
	int numberOfHeaders; ///< How many headers we have
	char** garbagePile; ///< Pointers that need to be freed
	int sizeOfGarbagePile; ///< Number of things in the garbage pile
} JamINI;

/// \brief Creates a new INI struct
/// \throws ERROR_ALLOC_FAILED
JamINI* jamINICreate();

/// \brief Loads an INI from a file
///
/// This function aims to follow the old Windows INI format from old as closely as possible.
/// Since there is quite a few different ini formats floating around, here is what this
/// particular loader does
///
///  + Headers are lines that start with '[' and end with ']'
///  + Keys are lines that have an '=' in them, everything after the first '=' is considered part of the value
///  + The loader does not care about "" in values and will simply call them part of the value
///  + Comments are lines that start with ';', no, '#' does not count as a comment
///  + Invalid lines in the file will be reported through jSetError as a warning
///
/// \throws ERROR_ALLOC_FAILED
/// \throws ERROR_OPEN_FAILED
JamINI* jamINILoad(const char *filename);

/// \brief This will print the INI file to a stream
///
/// If you want to save an INI file to load it later,
/// just open a file, pass it here, and close the file.
/// This function will properly print a formatted INI
/// file that loadINI will be able to read. It's also
/// good for debugging because you can print your INIs
/// to stdout/stderr/whatever.
///
/// \throws ERROR_NULL_POINTER
void jamINIOutput(JamINI *ini, FILE *stream);

/// \brief Set a key in an INI
/// \throws ERROR_ALLOC_FAILED
/// \throws ERROR_NULL_POINTER
void jamINISetKey(JamINI *ini, const char *header, const char *key, char *val);

/// \brief Gets a key from an INI
/// \throws ERROR_NULL_POINTER
char* jamINIGetKey(JamINI *ini, const char *header, const char *key, char *def);

/// \brief Destroys an INI and all memory it allocated
void jamINIFree(JamINI *ini);

#ifdef __cplusplus
}
#endif
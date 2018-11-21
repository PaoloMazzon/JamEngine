//
// Created by lugi1 on 2018-11-15.
//

#pragma once
#include <stdbool.h>

/// \brief A vector for C-strings
typedef struct {
	char** strList; ///< The list of c-strings
	bool* dynamic; ///< Weather or not a string is on the heap
	int size; ///< The size of the list in elements
} StringList;

/// \brief Creates a string list
StringList* createStringList();

/// \brief Reads a file line-by-line into a string list
/// 
/// This does not include newline characters
StringList* loadStringList(const char* fname);

/// \brief Appends a string to an string list
void appendStringToStringList(StringList* list, char* string, bool heapBased);

/// \brief Frees a string list as well as its contents
void freeStringList(StringList* list);
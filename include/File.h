/// \file File.h
/// \author lugi1
/// \brief Really just makes file management easier in C, because oh boy is it a pain

#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/// \brief A vector for C-strings
typedef struct {
	char** strList; ///< The list of c-strings
	bool* dynamic; ///< Weather or not a string is on the heap
	int size; ///< The size of the list in elements
} JamStringList;

/// \brief Creates a string list
/// \return Returns the new list or NULL
/// \throws ERROR_ALLOC_FAILED
JamStringList* jamStringListCreate();

/// \brief Reads a file line-by-line into a string list
/// \param fname The file to load as a list of strings
/// \return Returns a new list or NULL
/// 
/// This does not include newline characters
///
/// \throws ERROR_ALLOC_FAILED
/// \throws ERROR_OPEN_FAILED
JamStringList* jamStringListLoad(const char *fname);

/// \brief Appends a string to an string list
/// \param list The list to append to
/// \param string The string to append
/// \param heapBased Weather or not the string was dynamically decleared
/// 
/// If heapBased is set to true, the pointer is
/// now this struct's responsibilty, and will be
/// destroyed with the struct.
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_REALLOC_FAILED
void jamStringListAppend(JamStringList *list, char *string, bool heapBased);

/// \brief Separates a string based on a delimiter
/// \param string The string to explode
/// \param delim The delimiter
/// \param ignoreQuotes Weather or not delimiters in quotes count
/// \return Returns a delimited list or NULL
/// 
/// In case the purpose is confusing, if you give this function
/// input like this: explodeString("hello, world, 123", ',', false)
/// you will theoretically get back a list of ["hello", "world",
/// "123"]. If ignoreQuotes is true, then delimiters inside of
/// quotation marks will be ignored.
///
/// \throws ERROR_ALLOC_FAILED
JamStringList* jamStringExplode(const char *string, char delim, bool ignoreQuotes);

/// \brief Frees a string list as well as its contents
/// \param list The list to destroy
void jamStringListFree(JamStringList *list);

/// \brief Takes any form of a file and returns the file's name only
///
/// For example, "/home/somedude/myfile.tar.gz" would be converted
/// to "myfile.tar", or "thing.txt" would be "thing"
///
/// You are responsible for the memory allocated by this function
///
/// \throws ERROR_ALLOC_FAILED
const char* jamGetNameOfFile(const char* filename);
	
#ifdef __cplusplus
}
#endif

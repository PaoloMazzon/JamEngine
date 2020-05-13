/// \file StringUtil.h
/// \author plo, of course
/// \brief Some incredibly useful string stuff
#pragma once
#include <Constants.h>

#ifdef __cplusplus
extern "C" {
#endif

#define END_OF_STRING (-1)
#define LAST_OCCURRENCE (-1)
#define FIRST_OCCURRENCE 1
#define STRING_NOT_FOUND (-1)
#define DEFAULT_ALLOC_AMOUNT 10

/// \brief Makes it easier to work with strings
/// 
/// This thing was meant to work with ASCII, but nothing in
/// this class seems like it won't work with UTF-8. The
/// length function also accounts for UTF-8.
typedef struct {
	char *str; ///< The internal string
	int length; ///< How many characters are currently in use
	int size; ///< Total size of the array
	short allocAmount; ///< How many characters to allocate at a time
} JamStringBuilder;

/// \brief Creates a string builder
/// 
/// \return Returns the new builder or NULL if it failed
/// \throws ERROR_ALLOC_FAILED
JamStringBuilder *jamStringBuilderCreate();

/// \brief Creates a string builder with a string pre loaded into it
/// 
/// \param string The string to copy into the new builder
/// 
/// \return Returns the new builder or NULL if it failed
/// \throws ERROR_ALLOC_FAILED
JamStringBuilder *jamStringBuilderCreateFromString(const char *string);

/// \brief Inserts a string into a string builder starting at index
/// 
/// \param builder The builder to use
/// \param string The string to insert
/// \param index Where to insert the string or END_OF_STRING to append it
/// 
/// \return Returns whatever you gave for string
///
/// This function allocates new memory as needed automatically.
/// Also, this function returns whatever you pass for string
/// specifically so you do something like 
/// `free(insertStringIntoBuilder(builder, ftoa(100), END_OF_STRING));`
/// to make the whole process really easy. (This function will
/// not cause a fuss if you pass null pointers)
///
/// \throws ERROR_REALLOC_FAILED
/// \throws ERROR_NULL_POINTER
char *jamStringBuilderInsert(JamStringBuilder *builder, char *string, int index);

/// \brief Removes something from the builder
///
/// \param builder The builder to use
/// \param index The index of the character to remove or END_OF_STRING for the end of the string
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_OUT_OF_BOUNDS
void jamStringBuilderRemoveChar(JamStringBuilder *builder, int index);

/// \brief Finds a string in a string builder and returns it
/// 
/// \param builder The builder to look through
/// \param string The string to find
/// \param occurrencesRequired How many occurences of string in we want (or just use the two constants LAST_OCCURRENCE and FIRST_OCCURRENCE)
///
/// \return Returns the index where the string shows up or -1 if not found
/// 
/// occurencesRequired is how many times string has to show up before this function
/// recognizes it and returns the position. For example, a value of
/// FIRST_OCCURRENCE means that it will look for the first occurence of string.
/// A count of 4 would look for the fourth occurence of string and so
/// on. LAST_OCCURRENCE on count will tell this function to look for the LAST
/// occurence of string.
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_OUT_OF_BOUNDS
int jamStringBuilderFind(JamStringBuilder *builder, const char *string, int occurrencesRequired);

/// \brief Grabs a substring and returns it as a new builder
/// 
/// \param builder The builder to grab the substring from
/// \param index The starting index to grab the string from
/// \param length How many characters to grab or END_OF_STRING for the rest of the string
/// 
/// \return Returns the substring or NULL if it failed
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ALLOC_FAILED
/// \throws ERROR_OUT_OF_BOUNDS
JamStringBuilder *jamStringBuilderSubstring(JamStringBuilder *builder, int index, int length);

/// \brief Shrinks a string builder so it doesn't have any extra memory on the end
/// \param builder The builder to shrink
/// 
/// This function removes any extra memory from the end of the char
/// array, making the allocated memory only what it's using. If
/// allocAmount is 1, this function will do nothing.
///
/// \throws ERROR_NULL_POINTER
void jamStringBuilderShrink(JamStringBuilder *builder);

/// \brief Pulls a c-string from a builder
/// 
/// \param builder The string builder to grab from
/// 
/// \return Returns a classic c-style string
/// \throws ERROR_NULL_POINTER
///
/// The string this returns belongs to the builder and as
/// such it will be destroyed with the builder.
const char *jamStringBuilderGetArray(JamStringBuilder *builder);

/// \brief Counts the total characters in the builder
/// 
/// \param builder the string builder to count up
/// 
/// \return Returns the total character count or -1 on failure
/// 
/// The reason this function exists instead of just
/// taking from builder->length is because this function
/// accounts for UTF-8 characters and will not count
/// a single emoji or whatever as 4 characters.
///
/// \throws ERROR_NULL_POINTER
int jamStringBuilderLength(JamStringBuilder *builder);

/// \brief Frees a string builder
/// 
/// \param builder The builder to free
void jamStringBuilderFree(JamStringBuilder *builder);

/// \brief Grabs the full UTF-8 character at pos, moving pos up to the next
/// character and returning it
/// \param string C-style string
/// \param pos Position in string to grab unicode character, will be updated
/// \return The next unicode character or zero if the end of the string (or an error in the encoding)
uint32 jamStringNextUnicode(const char* string, int* pos);

#ifdef atof
#undef atof
#endif

/// \brief Converts a string to a float (Works only in base-10)
/// 
/// \param string The string that contains only numbers and a decimal
/// 
/// \return Returns a double with the converted number
double atof(const char *string);

#ifdef ftoa
#undef ftoa
#endif

/// \brief Converts a double to a string
/// 
/// \param input The number to be converted
/// 
/// \return Returns the processed string which will need to be manually freed
/// \throws ERROR_ALLOC_FAILED
char *ftoa(double input);

/// \brief Turns a string into a number from 0 to maxNumber
uint64 jamHashString(const char *string, uint64 maxNumber);

#ifdef __cplusplus
}
#endif
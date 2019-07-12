/// \page StringUtil.h
/// \author plo, of course
/// Some incredibly useful string stuff
#pragma once

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
	char* str; ///< The internal string
	int length; ///< How many characters are currently in use
	int size; ///< Total size of the array
	short allocAmount; ///< How many characters to allocate at a time
} StringBuilder;

/// \brief Creates a string builder
/// 
/// \return Returns the new builder or NULL if it failed
StringBuilder* createStringBuilder();

/// \brief Creates a string builder with a string pre loaded into it
/// 
/// \param string The string to copy into the new builder
/// 
/// \return Returns the new builder or NULL if it failed
StringBuilder* createBuilderFromString(const char* string);

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
char* insertStringIntoBuilder(StringBuilder* builder, char* string, int index);

/// \brief Removes something from the builder
///
/// \param builder The builder to use
/// \param index The index of the character to remove or END_OF_STRING for the end of the string
void removeCharFromBuilder(StringBuilder* builder, int index);

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
int findStringInBuilder(StringBuilder* builder, const char* string, int occurrencesRequired);

/// \brief Grabs a substring and returns it as a new builder
/// 
/// \param builder The builder to grab the substring from
/// \param index The starting index to grab the string from
/// \param length How many characters to grab or END_OF_STRING for the rest of the string
/// 
/// \return Returns the substring or NULL if it failed
StringBuilder* substringFromBuilder(StringBuilder* builder, int index, int length);

/// \brief Shrinks a string builder so it doesn't have any extra memory on the end
/// 
/// \param builder The builder to shrink
/// 
/// This function removes any extra memory from the end of the char
/// array, making the allocated memory only what it's using. If
/// allocAmount is 1, this function will do nothing.
void shrinkBuilder(StringBuilder* builder);

/// \brief Pulls a c-string from a builder
/// 
/// \param builder The string builder to grab from
/// 
/// \return Returns a classic c-style string
///
/// The string this returns belongs to the builder and as
/// such it will be destroyed with the builder.
const char* getBuilderArray(StringBuilder* builder);

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
int getBuilderLength(StringBuilder* builder);

/// \brief Frees a string builder
/// 
/// \param builder The builder to free
void freeStringBuilder(StringBuilder* builder);

#ifdef atof
#undef atof
#endif
/// \brief Converts a string to a float (Works only in base-10)
/// 
/// \param string The string that contains only numbers and a decimal
/// 
/// \return Returns a double with the converted number
double atof(const char* string);

#ifdef ftoa
#undef ftoa
#endif
/// \brief Converts a double to a string
/// 
/// \param input The number to be converted
/// 
/// \return Returns the processed string which will need to be manually freed
char* ftoa(double input);
#include "StringUtil.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <malloc.h>
#include "JamError.h"

static inline bool _utf8OneByte(char c) {
	return ((c & 0b10000000) == 0b00000000) && (c != 0);
}

static inline bool _utf8TwoBytes(char c) {
	return (c & 0b11100000) == 0b11000000;
}

static inline bool _utf8ThreeBytes(char c) {
	return (c & 0b11110000) == 0b11100000;
}

static inline bool _utf8FourBytes(char c) {
	return (c & 0b11111000) == 0b11110000;
}

static inline bool _utf8ContinuationChar(char c) {
	return (c & 0b11000000) == 0b10000000;
}

///////////////////////////////////////////////////////////////
JamStringBuilder* jamStringBuilderCreate() {
	JamStringBuilder* builder = calloc(sizeof(JamStringBuilder), 1);

	if (builder != NULL) {
		builder->allocAmount = DEFAULT_ALLOC_AMOUNT;
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Could not create builder (jamStringBuilderCreate)");
	}

	return builder;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamStringBuilder* jamStringBuilderCreateFromString(const char *string) {
	JamStringBuilder* builder = jamStringBuilderCreate();

	if (builder != NULL) {
		jamStringBuilderInsert(builder, (char *) string, -1);
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to create builder (jamStringBuilderCreateFromString)");
	}

	return builder;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
char* jamStringBuilderInsert(JamStringBuilder *builder, char *string, int index) {
	int stringLen;
	int i;
	char* newString;
	bool goodToProceed = true;

	if (builder != NULL && string != NULL) {
		stringLen = strlen(string);

		// In case they said at the end of the string
		if (index == END_OF_STRING)
			index = builder->length;

		// Is there already enough space in the builder for it?
		if (stringLen > builder->size - builder->length - 1) {
			newString = (char*)realloc(builder->str, builder->size + builder->allocAmount + stringLen);

			// Assuming we allocated the new bit fine, place a tailing zero and update the size
			if (newString != NULL) {
				builder->str = newString;
				builder->size += builder->allocAmount + stringLen;
				builder->str[builder->length + stringLen] = 0;
			} else {
				jSetError(ERROR_REALLOC_FAILED, "Failed to reallocate builder (jamStringBuilderInsert)");
				goodToProceed = false;
			}
		}

		if (goodToProceed) {
			builder->length += stringLen;

			// Move the old bits ahead
			for (i = builder->length - 1; i >= index + stringLen; i--)
				builder->str[i] = builder->str[i - stringLen];
		
			// Place the new bits in
			for (i = index; i < index + stringLen; i++)
				builder->str[i] = string[i - index];
		}
	} else {
		if (builder == NULL)
			jSetError(ERROR_NULL_POINTER, "Builder does not exist (jamStringBuilderInsert)");
		if (string == NULL)
			jSetError(ERROR_NULL_POINTER, "String does not exist (jamStringBuilderInsert)");
	}

	return string;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void jamStringBuilderRemoveChar(JamStringBuilder *builder, int index) {
	int i;
	if (builder != NULL && index < builder->length && index >= -1) {
		if (builder->length > 0) {
			if (index == END_OF_STRING)
				index = builder->length - 1;

			// Push every character starting at index 1 back
			for (i = index; i < builder->length - 1; i++)
				builder->str[i] = builder->str[i + 1];

			// Decrement length then place a tailing zero at the end
			builder->str[builder->length - 1] = 0;
			builder->length--;
		}
	} else {
		if (builder == NULL) {
			jSetError(ERROR_NULL_POINTER, "Builder does not exist (jamStringBuilderRemoveChar)");
		} else {
			jSetError(ERROR_OUT_OF_BOUNDS, "Index out of bounds (jamStringBuilderRemoveChar)");
		}
	}
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
int jamStringBuilderFind(JamStringBuilder *builder, const char *string, int occurrencesRequired) {
	int i = 0;
	int count = 0;
	int finalReturn = STRING_NOT_FOUND;
	int posInStr = 0;
	int lastPos = STRING_NOT_FOUND;
	int stringLength;
	
	if (builder != NULL && occurrencesRequired > -2 && string != NULL) {
		stringLength = strlen(string);
		
		if (builder->length > 0 && stringLength > 0) {
			while (i < builder->length && (count < occurrencesRequired || occurrencesRequired == LAST_OCCURRENCE)) {
				// Continuation
				if (string[posInStr] == builder->str[i] && posInStr < stringLength - 1) {
					posInStr++;
				} else if (string[posInStr] == builder->str[i] && posInStr == stringLength - 1) {
					posInStr = 0;
					count++;
					lastPos = i - stringLength + 1;
				}
				i++;
			}
		}

		// Calculate out the results
		if (count == occurrencesRequired || occurrencesRequired == LAST_OCCURRENCE)
			finalReturn = lastPos;
	} else {
		if (builder == NULL)
			jSetError(ERROR_NULL_POINTER, "Builder does not exist (jamStringBuilderRemoveChar)");
		if (occurrencesRequired < -1) {
			jSetError(ERROR_OUT_OF_BOUNDS, "Occurrences required out of bounds (jamStringBuilderRemoveChar)");
		}
		if (string == NULL)
			jSetError(ERROR_NULL_POINTER, "String does not exist (jamStringBuilderRemoveChar)");
	}

	return finalReturn;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamStringBuilder* jamStringBuilderSubstring(JamStringBuilder *builder, int index, int length) {
	JamStringBuilder* sub = jamStringBuilderCreate();
	int i;

	if (builder != NULL && sub != NULL && index >= 0 && index + length <= builder->length && length > -2) {
		if (length == END_OF_STRING)
			length = builder->length - index;

		sub->str = (char*)malloc(length + 1);

		// If malloc worked, we copy the contents and setup sub's meta info
		if (sub->str != NULL) {
			for (i = index; i < index + length; i++)
				sub->str[i - index] = builder->str[i];
			sub->str[length] = 0;
			sub->size = length;
			sub->length = length;
		} else {
			jamStringBuilderFree(sub);
			sub = NULL;
			jSetError(ERROR_ALLOC_FAILED, "Failed to create internal string (jamStringBuilderSubstring)");
		}
	} else {
		jamStringBuilderFree(sub);
		if (sub == NULL) {
			jSetError(ERROR_ALLOC_FAILED, "Failed to create substring (jamStringBuilderSubstring)");
		}
		if (builder == NULL) {
			jSetError(ERROR_NULL_POINTER, "Builder does not exist (jamStringBuilderSubstring)");
		}
		if (builder != NULL && sub != NULL) {
			jSetError(ERROR_OUT_OF_BOUNDS, "Index out of bounds (jamStringBuilderSubstring)");
		}
		sub = NULL;
	}

	return sub;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void jamStringBuilderShrink(JamStringBuilder *builder) { // NOTE: UNTESTED
	char* newArray;
	if (builder != NULL) {
		if (builder->size > builder->length + 1) {
			newArray = (char*)realloc(builder->str, builder->length + 1);

			if (newArray != NULL) {
				builder->str = newArray;
				builder->str[builder->length - 1] = 0;
				builder->size = builder->length + 1;
			}
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Builder does not exist (jamStringBuilderShrink)");
	}
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
int jamStringBuilderLength(JamStringBuilder *builder) {
	int i;
	int count = 0;
	if (builder != NULL) {
		// As long as its not a UTF-8 continuation character, we count it
		for (i = 0; i < builder->length; i++)
			if (!((builder->str[i] & 128) == 128 && (builder->str[i] & 64) == 0))
				count++;
	} else {
		count = -1;
		jSetError(ERROR_NULL_POINTER, "Builder does not exist (jamStringBuilderLength)");
	}

	return count;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
const char* jamStringBuilderGetArray(JamStringBuilder *builder) {
	const char* ret = NULL;
	if (builder != NULL) {
		ret = (const char*)builder->str;
	} else {
		jSetError(ERROR_NULL_POINTER, "Builder does not exist (jamStringBuilderGetArray)");
	}

	return ret;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void jamStringBuilderFree(JamStringBuilder *builder) {
	if (builder != NULL) {
		free(builder->str);
		free(builder);
	}
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
uint32 jamStringNextUnicode(const char* string, int* pos) {
	// This looks like a wall of code, but its really quite simple.
	// Given the current position, it checks what kind of character
	// it is and makes sure it has all the necessary utf-8 continuation
	// characters for this header. Once it knows for certain what
	// kind of character this is, it can do some bit-wise trickery
	// to assemble the whole unicode character. If the character at
	// pos is zero or there is a utf-8 error, zero will be returned
	// because all of the utf-8 checking functions (UTF_8_*) will return
	// false.
	uint32 unicode = 0;

	if (_utf8OneByte(string[*pos])) {
		unicode = (uint32)string[*pos];
		(*pos)++;
	} else if (_utf8TwoBytes(string[*pos])
			&& _utf8ContinuationChar(string[(*pos) + 1])) {
		unicode = ((uint32)string[*pos] & 0b00011111) << 6;
		unicode += (uint32)string[(*pos) + 1] & 0b00111111;
		(*pos) += 2;
	} else if (_utf8ThreeBytes(string[*pos])
			&& _utf8ContinuationChar(string[(*pos) + 1])
			&& _utf8ContinuationChar(string[(*pos) + 2])) {
		unicode = ((uint32)string[*pos] & 0b00001111) << 12;
		unicode |= ((uint32)string[(*pos) + 1] & 0b00111111) << 6;
		unicode |= (uint32)string[(*pos) + 2] & 0b00111111;
		(*pos) += 3;
	} else if (_utf8FourBytes(string[*pos])
			&& _utf8ContinuationChar(string[(*pos) + 1])
			&& _utf8ContinuationChar(string[(*pos) + 2])
			&& _utf8ContinuationChar(string[(*pos) + 3])) {
		unicode = ((uint32)string[*pos] & 0b00001111) << 18;
		unicode += ((uint32)string[(*pos) + 1] & 0b00111111) << 12;
		unicode += ((uint32)string[(*pos) + 2] & 0b00111111) << 6;
		unicode += (uint32)string[(*pos) + 3] & 0b00111111;
		(*pos) += 4;
	}

	return unicode;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
double atof(const char* string) {
	double output = 0;
	int i;
	int inputLength = strlen(string);
	int decimalPosition = -1;
	bool errorInString = false;
	int negative = 0;

	if (inputLength > 0) {
		// Account for negatives
		if (string[0] == '-')
			negative = 1;

		// Make sure there is a proper number here
		for (i = negative; i < inputLength; i++) {
			if ((string[i] > 57 || string[i] < 48) && string[i] != 46 && string[i] != 44)
				errorInString = true;
			else if ((string[i] == 46 || string[i] == 44) && decimalPosition != -1)
				errorInString = true;
			else if (string[i] == 46 || string[i] == 44)
				decimalPosition = i;
		}

		// We continue if the string checks out
		if (!errorInString) {
			if (decimalPosition == -1)
				decimalPosition = inputLength;

			// Calculate everything before the decimal
			for (i = negative; i < decimalPosition; i++) {
				output += pow(10, (decimalPosition - 1) - i) * (string[i] - 48);
			}

			// Now everything after the decimal
			for (i = decimalPosition + 1; i < inputLength; i++) {
				output += pow(10, decimalPosition - i) * (string[i] - 48);
			}
		}

		if (negative == 1)
			output *= -1;
	}

	return output;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
char* ftoa(double input) {
	int i;
	int numberOfCharacters;
	int negative = 0;
	bool addedToDecimal = false;

	if (input < 0) {
		input = input * -1;
		negative = 1;
	}

	// This algorithm doesn't like numbers in the range (0, 1) with no leading number
	// in the ones or tenths place so this fixes it (along with replacing this with a
	// 0 at the end of this function but before negatives are handled)
	if (input < 1)  {
		input += 5; // This 5 is completely arbitrary is just needs to be between 1 and 9
		addedToDecimal = true;
	}

	// Calculate the special case for powers of 10 (1, 10, 100, 1000...)
	if (input == pow(10, ceil(log10(input))))
		numberOfCharacters = (int)ceil(log10(input)) + 1;
	else
		numberOfCharacters = (int)ceil(log10(input));

	// Declare rest of variables
	int numWithoutDecimals = numberOfCharacters;
	double threshHold = 0.0000005;
	double findDecimalPlaces = input;
	char* string;
	float subtractFromInput;
	
	// Count up amount of decimal numbers
	while (fabs(findDecimalPlaces - round(findDecimalPlaces)) > threshHold) {
		findDecimalPlaces *= 10;
		threshHold *= 10;
		numberOfCharacters++;
	}
	
	// Allocate the string - we may or may not need space for a decimal point
	if (numWithoutDecimals < numberOfCharacters)
		string = (char*)calloc(1, numberOfCharacters + 2 + negative);
	else
		string = (char*)calloc(1, numberOfCharacters + 1 + negative);

	if (string != NULL) {
		// First we deal with before the decimal
		for (i = 0; i < numWithoutDecimals; i++) {
			subtractFromInput = pow(10, numWithoutDecimals - 1 - i);
			string[i] = floor(input / subtractFromInput) + 48;
			input -= floor(input / subtractFromInput) * subtractFromInput;
		}

		// Only do after decimal if there is an after decimal
		if (numWithoutDecimals < numberOfCharacters) {
			// Tack on a decimal place to the end
			string[i] = '.';

			// Work from after decimal point
			for (i = numWithoutDecimals + 1; i <= numberOfCharacters; i++) {
				if (i == numberOfCharacters)
					input += 0.01;
				subtractFromInput = input * 10;//pow(10, i - numWithoutDecimals)
				string[i] = floor(subtractFromInput) + 48;
				input = (subtractFromInput) - floor(subtractFromInput);
			}
		}

		// Just in case we threw something before a decimal, replace it with a zero
		if (addedToDecimal)
			string[0] = '0';

		// Now that the string is assembled, if we're negative, we will now push
		// everything 1 back and insert a negative sign
		if (negative) {
			for (i = strlen(string); i > 0; i--)
				string[i] = string[i - 1];
			string[0] = '-';
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate string with decimal %f (ftoa)", input);
	}

	return string;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// djb2 Algorithm by Dan Bernstein
uint64 jamHashString(const char* string, uint64 maxNumber) {
	uint64 hash = 5381;
	int i = 0;

	while (string[i] != 0) {
		hash = ((hash << 5) + hash) + string[i]; /* hash * 33 + c */
		i++;
	}

	return hash % maxNumber;
}
///////////////////////////////////////////////////////////////
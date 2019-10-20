#include "StringUtil.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <malloc.h>
#include "JamError.h"

///////////////////////////////////////////////////////////////
JamStringBuilder* jamCreateStringBuilder() {
	JamStringBuilder* builder = calloc(sizeof(JamStringBuilder), 1);

	if (builder != NULL) {
		builder->allocAmount = DEFAULT_ALLOC_AMOUNT;
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Could not create builder (jamCreateStringBuilder)\n");
	}

	return builder;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamStringBuilder* jamCreateBuilderFromString(const char *string) {
	JamStringBuilder* builder = jamCreateStringBuilder();

	if (builder != NULL) {
		jamInsertStringIntoBuilder(builder, (char *) string, -1);
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to create builder (jamCreateBuilderFromString)\n");
	}

	return builder;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
char* jamInsertStringIntoBuilder(JamStringBuilder *builder, char *string, int index) {
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
				jSetError(ERROR_REALLOC_FAILED, "Failed to reallocate builder (jamInsertStringIntoBuilder)\n");
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
			jSetError(ERROR_NULL_POINTER, "Builder does not exist (jamInsertStringIntoBuilder)\n");
		if (string == NULL)
			jSetError(ERROR_NULL_POINTER, "String does not exist (jamInsertStringIntoBuilder)\n");
	}

	return string;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void jamRemoveCharFromBuilder(JamStringBuilder *builder, int index) {
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
			jSetError(ERROR_NULL_POINTER, "Builder does not exist (jamRemoveCharFromBuilder)\n");
		} else {
			jSetError(ERROR_OUT_OF_BOUNDS, "Index out of bounds (jamRemoveCharFromBuilder)\n");
		}
	}
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
int jamFindStringInBuilder(JamStringBuilder *builder, const char *string, int occurrencesRequired) {
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
			jSetError(ERROR_NULL_POINTER, "Builder does not exist (jamRemoveCharFromBuilder)\n");
		if (occurrencesRequired < -1) {
			jSetError(ERROR_OUT_OF_BOUNDS, "Occurrences required out of bounds (jamRemoveCharFromBuilder)\n");
		}
		if (string == NULL)
			jSetError(ERROR_NULL_POINTER, "String does not exist (jamRemoveCharFromBuilder)\n");
	}

	return finalReturn;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamStringBuilder* jamSubstringFromBuilder(JamStringBuilder *builder, int index, int length) {
	JamStringBuilder* sub = jamCreateStringBuilder();
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
			jamFreeStringBuilder(sub);
			sub = NULL;
			jSetError(ERROR_ALLOC_FAILED, "Failed to create internal string (jamSubstringFromBuilder)\n");
		}
	} else {
		jamFreeStringBuilder(sub);
		if (sub == NULL) {
			jSetError(ERROR_ALLOC_FAILED, "Failed to create substring (jamSubstringFromBuilder)\n");
		}
		if (builder == NULL) {
			jSetError(ERROR_NULL_POINTER, "Builder does not exist (jamSubstringFromBuilder)\n");
		}
		if (builder != NULL && sub != NULL) {
			jSetError(ERROR_OUT_OF_BOUNDS, "Index out of bounds (jamSubstringFromBuilder)\n");
		}
		sub = NULL;
	}

	return sub;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void jamShrinkBuilder(JamStringBuilder *builder) { // NOTE: UNTESTED
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
		jSetError(ERROR_NULL_POINTER, "Builder does not exist (jamShrinkBuilder)\n");
	}
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
int jamGetBuilderLength(JamStringBuilder *builder) {
	int i;
	int count = 0;
	if (builder != NULL) {
		// As long as its not a UTF-8 continuation character, we count it
		for (i = 0; i < builder->length; i++)
			if (!((builder->str[i] & 128) == 128 && (builder->str[i] & 64) == 0))
				count++;
	} else {
		count = -1;
		jSetError(ERROR_NULL_POINTER, "Builder does not exist (jamGetBuilderLength)\n");
	}

	return count;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
const char* jamGetBuilderArray(JamStringBuilder *builder) {
	const char* ret = NULL;
	if (builder != NULL) {
		ret = (const char*)builder->str;
	} else {
		jSetError(ERROR_NULL_POINTER, "Builder does not exist (jamGetBuilderArray)\n");
	}

	return ret;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void jamFreeStringBuilder(JamStringBuilder *builder) {
	if (builder != NULL) {
		free(builder->str);
		free(builder);
	}
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
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate string with decimal %f (ftoa)\n", input);
	}

	return string;
}
///////////////////////////////////////////////////////////////
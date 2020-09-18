/// \file JSON.h
/// \author Paolo Mazzon
/// \brief Funtionality for JSON parsing
#pragma once
#include <stdbool.h>
#include "Constants.h"

typedef struct JamJSONObject JamJSONObject;
typedef struct JamJSONValue JamJSONValue;

/// \brief Types that a JSON object can be
typedef enum {
	ot_String = 0,
	ot_Real = 1,
	ot_Bool = 2,
	ot_Object = 3,
	ot_Null = 4
} JamJSONObjectType;

/// \brief A value in a JSON object
struct JamJSONValue {
	union {
		const char *string;    ///< String value
		double real;           ///< Number value
		bool boolean;          ///< Boolean value
		JamJSONObject *object; ///< Another object as a value
		void *nil;             ///< This is NULL if the type is ot_Null
	} Value;
	const char *key;        ///< Name of this object
	JamJSONObjectType type; ///< Type of JSON object this is
};

/// \brief A JSON object
///
/// The initial file loaded will have a NULL key.
struct JamJSONObject {
	JamJSONValue *values; ///< Vector of values
	uint32 size;          ///< Number of values in this object
};

/// \brief Creates an empty json object
JamJSONObject *jamJSONParse(const char *filename);

/// \brief Frees a single json object
void jamJSONFree(JamJSONObject *object);
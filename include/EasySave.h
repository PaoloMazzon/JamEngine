/// \file EasySave.h
/// \author Plo
/// \brief Easily save and load data from files

#include <Constants.h>
#pragma once

// For data type checking
typedef enum {
	dt_DoubleVal = 0,
	dt_Uint64Val = 1,
	dt_Uint32Val = 2,
	dt_Uint16Val = 3,
	dt_Uint8Val = 4,
	dt_Sint64Val = 5,
	dt_Sint32Val = 6,
	dt_Sint16Val = 7,
	dt_Sint8Val = 8,
	dt_StringVal = 9,
	dt_BytesVal = 10
} _JamEasyDataType;

// A piece of data in the easy saver
typedef struct {
	_JamEasyDataType type; ///< Type of data this is
	const char* key;

	union {
		double doubleVal;
		uint64 uint64Val;
		uint32 uint32Val;
		uint16 uint16Val;
		uint8 uint8Val;
		sint64 sint64Val;
		sint32 sint32Val;
		sint16 sint16Val;
		sint8 sint8Val;
		const char* stringVal;
		struct {
			uint32 size;
			void* data;
		};
	};
} _JamEasyData;

/// \brief Data surrounding the saving and loading of things easily
///
/// JamBuffers store data in files and load from them a lot quicker
/// than these are able to, however, loading and saving with buffers
/// can be quite tedious. For small things like settings and config
/// files these are just fine and much less headache.
///
/// These also function like files, you open a specific easy save,
/// and if it doesn't exist you get a blank one. After that, you can
/// flush it to save but you usually just close the easy save to save
/// it to disk.
///
/// It is also worth noting that the easy save makes its own copies
/// of everything (keys, strings, variable length data), and as such
/// they will all be freed when the easy save is closed.
///
/// Example:
///
/// 	JamEasySave* ez = jamEasySaveLoad("filename.txt");
/// 	player.maxHp = jamEasySaveGetDouble(ez, "maxHp");
/// 	jamEasySaveSetString(ez, "version", "1.0.4");
/// 	jamEasySaveClose(ez);
///
typedef struct {
	uint32 size; ///< Amount of data in this easy save
	_JamEasyData** data; ///< Vector of all the data
	const char* filename; ///< Filename of this save
} JamEasySave;

/// \brief Loads an easy save or returns an empty one
///
/// For reference, here is an overview of how easy save files are
/// structured:
///
///  + 4 bytes at the top representing how many entries there are
///  + For each entry, the following
///    + 2 bytes for the length of the key with terminating 0
///    + 4 byte for the type of entry this is (string, uint8...) (this could be 1 byte but enums are ints)
///    + 4 bytes for the size of the entry (in case its variable length)
///    + x bytes for this entry's key (with terminating 0)
///    + x bytes for the value of this entry
///
/// Since these save as binary files, it essentially means that these
/// files are not human read-able or modifiable. That by no means implies
/// that this is some sort of save-file-encryption and should not
/// be treated as such. If you would like something that a user can
/// open up and tweak without a hex editor and this documentation,
/// consider using a JamINI instead (although they are a bit more
/// work to use).
JamEasySave* jamEasySaveLoad(const char* filename);

/// brief Gets a double from an easy save
double jamEasySaveGetDouble(JamEasySave* easySave, const char* key);

/// \brief Sets a value in an easy save
void jamEasySaveSetDouble(JamEasySave* easySave, const char* key, double val);

/// brief Gets a uint64 from an easy save
uint64 jamEasySaveGetUint64(JamEasySave* easySave, const char* key);

/// \brief Sets a value in an easy save
void jamEasySaveSetUint64(JamEasySave* easySave, const char* key, uint64 val);

/// brief Gets a uint32 from an easy save
uint32 jamEasySaveGetUint32(JamEasySave* easySave, const char* key);

/// \brief Sets a value in an easy save
void jamEasySaveSetUint32(JamEasySave* easySave, const char* key, uint32 val);

/// brief Gets a uint16 from an easy save
uint16 jamEasySaveGetUint16(JamEasySave* easySave, const char* key);

/// \brief Sets a value in an easy save
void jamEasySaveSetUint16(JamEasySave* easySave, const char* key, uint16 val);

/// brief Gets a uint8 from an easy save
uint8 jamEasySaveGetUint8(JamEasySave* easySave, const char* key);

/// \brief Sets a value in an easy save
void jamEasySaveSetUint8(JamEasySave* easySave, const char* key, uint8 val);

/// brief Gets a sint64 from an easy save
sint64 jamEasySaveGetSint64(JamEasySave* easySave, const char* key);

/// \brief Sets a value in an easy save
void jamEasySaveSetSint64(JamEasySave* easySave, const char* key, sint64 val);

/// brief Gets a sint32 from an easy save
sint32 jamEasySaveGetSint32(JamEasySave* easySave, const char* key);

/// \brief Sets a value in an easy save
void jamEasySaveSetSint32(JamEasySave* easySave, const char* key, sint32 val);

/// brief Gets a sint16 from an easy save
sint16 jamEasySaveGetSint16(JamEasySave* easySave, const char* key);

/// \brief Sets a value in an easy save
void jamEasySaveSetSint16(JamEasySave* easySave, const char* key, sint16 val);

/// brief Gets a sint8 from an easy save
sint8 jamEasySaveGetSint8(JamEasySave* easySave, const char* key);

/// \brief Sets a value in an easy save
void jamEasySaveSetSint8(JamEasySave* easySave, const char* key, sint8 val);

/// brief Gets a string from an easy save
const char* jamEasySaveGetString(JamEasySave* easySave, const char* key);

/// \brief Sets a value in an easy save
void jamEasySaveSetString(JamEasySave* easySave, const char* key, const char* val);

/// \brief Gets variable length data from an easy save
void* jamEasySaveGetData(JamEasySave* easySave, const char* key);

/// \brief Sets variable length data in the easy save
void jamEasySaveSetData(JamEasySave* easySave, const char* key, void* data, uint32 size);

/// \brief Saves an easy save to file without freeing the struct
void jamEasySaveFlush(JamEasySave* easySave);

/// \brief Saves the easy save to its file and frees the struct
void jamEasySaveClose(JamEasySave* easySave);
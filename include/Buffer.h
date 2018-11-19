//////////////////////////////////////////////////////////
// Declares the buffer struct and functions
//
// Copyright (c) Paolo Mazzon
// All rights reserved
//////////////////////////////////////////////////////////

#pragma once
#include "Constants.h"
#include <stdbool.h>

////////////////////////////////////////////////
/// \brief A binary buffer for storing info
///
////////////////////////////////////////////////
typedef struct {
	uint8* buffer; ///< The actual binary buffer
	uint64 size; ///< The size of the buffer
	uint64 pointer; ///< Where in the buffer we are
} Buffer;

////////////////////////////////////////////////
/// \brief Creates a new buffer
///
/// \param size The size of said buffer in bytes
///
/// \return Returns a new buffer of size 'size'
////////////////////////////////////////////////
Buffer* createBuffer(uint64 size);

////////////////////////////////////////////////
/// \brief Creates a new buffer
///
/// \param filename Name of the binary file
///
/// \return Returns a new buffer with the file
////////////////////////////////////////////////
Buffer* loadBuffer(const char* filename);

////////////////////////////////////////////////
/// \brief Frees a buffer
///
/// \param buffer The buffer to free
////////////////////////////////////////////////
void freeBuffer(Buffer* buffer);

////////////////////////////////////////////////
/// \brief Resizes a buffer
///
/// \param buffer The buffer to resize
/// \param newSize The buffer's new size
///
/// \return Returns false if failed
////////////////////////////////////////////////
bool resizeBuffer(Buffer* buffer, uint64 newSize);

////////////////////////////////////////////////
/// \brief Zeroes a buffer
///
/// \param buffer The buffer to zero
////////////////////////////////////////////////
void zeroBuffer(Buffer* buffer);

////////////////////////////////////////////////
/// \brief Places a byte at the pointer
///
/// \param buffer The buffer to use
/// \param byte The byte to place
///
/// \return Returns false if failed
////////////////////////////////////////////////
bool addByte1(Buffer* buffer, uint8 byte);

////////////////////////////////////////////////
/// \brief Places bytes at the pointer
///
/// \param buffer The buffer to use
/// \param bytes The byte to place
///
/// \return Returns false if failed
////////////////////////////////////////////////
bool addByte2(Buffer* buffer, uint16 bytes);

////////////////////////////////////////////////
/// \brief Places bytes at the pointer
///
/// \param buffer The buffer to use
/// \param bytes The byte to place
///
/// \return Returns false if failed
////////////////////////////////////////////////
bool addByte4(Buffer* buffer, uint32 bytes);

////////////////////////////////////////////////
/// \brief Places bytes at the pointer
///
/// \param buffer The buffer to use
/// \param bytes The bytes to place
///
/// \return Returns false if failed
////////////////////////////////////////////////
bool addByte8(Buffer* buffer, uint64 bytes);

////////////////////////////////////////////////
/// \brief Reads a byte at the pointer
///
/// \param buffer The buffer to use
/// \param defaultReturn the default value to return
///
/// \return Returns the read value or defaultReturn
////////////////////////////////////////////////
uint8 readByte1(Buffer* buffer, uint8 defaultReturn);

////////////////////////////////////////////////
/// \brief Reads bytes at the pointer
///
/// \param buffer The buffer to use
/// \param defaultReturn the default value to return
///
/// \return Returns the read value or defaultReturn
////////////////////////////////////////////////
uint16 readByte2(Buffer* buffer, uint16 defaultReturn);

////////////////////////////////////////////////
/// \brief Reads bytes at the pointer
///
/// \param buffer The buffer to use
/// \param defaultReturn the default value to return
///
/// \return Returns the read value or defaultReturn
////////////////////////////////////////////////
uint32 readByte4(Buffer* buffer, uint32 defaultReturn);

////////////////////////////////////////////////
/// \brief Reads bytes at the pointer
///
/// \param buffer The buffer to use
/// \param defaultReturn the default value to return
///
/// \return Returns the read value or defaultReturn
////////////////////////////////////////////////
uint64 readByte8(Buffer* buffer, uint64 defaultReturn);

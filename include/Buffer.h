/// \file Buffer.h
/// \author lugi1
/// \brief This is a tool to make binary data easy(er) to handle
///
/// Do note that this is still a complex tool that is used
/// to make things either quicker or take up less space. This
/// tool is extremely light since it is really just function
/// calls and bit-shifting, so use it freely.

#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// We need to know size
#ifndef uint8
typedef unsigned char uint8;
#endif
#ifndef uint16
typedef unsigned short uint16;
#endif
#ifndef uint32
typedef unsigned int uint32;
#endif
#ifndef uint64
typedef unsigned long long uint64;
#endif

////////////////////////////////////////////////
/// \brief A binary buffer for storing info
///
/// \warning Functions surrounding this buffer,
/// unlike other functions in jam engine, are not
/// safe to give null pointers for the sake of
/// this binary buffer being lightning fast. If
/// pass a null pointer to any functions here your
/// program will crash. Make sure you check the
/// buffer to not be null before you work with it.
///
////////////////////////////////////////////////
typedef struct {
	uint8* buffer; ///< The actual binary buffer
	uint64 size; ///< The size of the buffer
	uint64 pointer; ///< Where in the buffer we are
} JamBuffer;

////////////////////////////////////////////////
/// \brief Creates a new buffer
///
/// \param size The size of said buffer in bytes
///
/// \return Returns a new buffer of size 'size'
///
/// \throws ERROR_ALLOC_FAILED
////////////////////////////////////////////////
JamBuffer* jamBufferCreate(uint64 size);

////////////////////////////////////////////////
/// \brief Creates a new buffer
///
/// \param filename Name of the binary file
///
/// \return Returns a new buffer with the file
///
/// If the file doesn't exist an empty buffer will
/// be returned.
///
/// \throws ERROR_FILE_FAILED
/// \throws ERROR_ALLOC_FAILED
////////////////////////////////////////////////
JamBuffer* jamBufferLoad(const char *filename);

/// \brief Places the contents of a buffer in a binary
void jamBufferSave(JamBuffer *buffer, const char *filename);

////////////////////////////////////////////////
/// \brief Frees a buffer
///
/// \param buffer The buffer to free
////////////////////////////////////////////////
void jamBufferFree(JamBuffer *buffer);

////////////////////////////////////////////////
/// \brief Resizes a buffer
///
/// \param buffer The buffer to resize
/// \param newSize The buffer's new size
///
/// This function has the side effect of setting the buffer's
/// pointer to zero
///
/// \throws ERROR_REALLOC_FAILED
////////////////////////////////////////////////
void jamBufferResize(JamBuffer *buffer, uint64 newSize);

/// \brief Sets the buffer pointer to a new location
void jamBufferSeek(JamBuffer* buffer, uint64 position);

////////////////////////////////////////////////
/// \brief Zeroes a buffer
///
/// \param buffer The buffer to zero
////////////////////////////////////////////////
void jamBufferZero(JamBuffer *buffer);

////////////////////////////////////////////////
/// \brief Places a byte at the pointer
///
/// \param buffer The buffer to use
/// \param byte The byte to place
///
/// data should be at least 1 byte long or the program
/// will crash
///
/// \throws ERROR_OUT_OF_BOUNDS
////////////////////////////////////////////////
void jamBufferAddByte1(JamBuffer *buffer, void *data);

////////////////////////////////////////////////
/// \brief Places bytes at the pointer
///
/// \param buffer The buffer to use
/// \param bytes The byte to place
///
/// data should be at least 2 byte long or the program
/// will crash
///
/// \throws ERROR_OUT_OF_BOUNDS
////////////////////////////////////////////////
void jamBufferAddByte2(JamBuffer *buffer, void* data);

////////////////////////////////////////////////
/// \brief Places bytes at the pointer
///
/// \param buffer The buffer to use
/// \param bytes The byte to place
///
/// data should be at least 4 byte long or the program
/// will crash
///
/// \throws ERROR_OUT_OF_BOUNDS
////////////////////////////////////////////////
void jamBufferAddByte4(JamBuffer *buffer, void *data);

////////////////////////////////////////////////
/// \brief Places bytes at the pointer
///
/// \param buffer The buffer to use
/// \param bytes The bytes to place
///
/// data should be at least 8 byte long or the program
/// will crash
///
/// \throws ERROR_OUT_OF_BOUNDS
////////////////////////////////////////////////
void jamBufferAddByte8(JamBuffer *buffer, void *data);

/// \brief Adds a variable amount of data to a buffer
void jamBufferAddByteX(JamBuffer *buffer, void *data, uint32 size);

////////////////////////////////////////////////
/// \brief Reads a byte at the pointer
///
/// \param buffer The buffer to use
/// \param defaultReturn the default value to return
///
/// \return Returns the read value or defaultReturn
///
/// data should be at least 1 byte long or the program
/// will crash
////////////////////////////////////////////////
void jamBufferReadByte1(JamBuffer *buffer, void *data);

////////////////////////////////////////////////
/// \brief Reads bytes at the pointer
///
/// \param buffer The buffer to use
/// \param defaultReturn the default value to return
///
/// \return Returns the read value or defaultReturn
///
/// data should be at least 2 byte long or the program
/// will crash
////////////////////////////////////////////////
void jamBufferReadByte2(JamBuffer *buffer, void *data);

////////////////////////////////////////////////
/// \brief Reads bytes at the pointer
///
/// \param buffer The buffer to use
/// \param defaultReturn the default value to return
///
/// \return Returns the read value or defaultReturn
///
/// data should be at least 4 byte long or the program
/// will crash
////////////////////////////////////////////////
void jamBufferReadByte4(JamBuffer *buffer, void *data);

////////////////////////////////////////////////
/// \brief Reads bytes at the pointer
///
/// \param buffer The buffer to use
/// \param defaultReturn the default value to return
///
/// \return Returns the read value or defaultReturn
///
/// data should be at least 8 byte long or the program
/// will crash
////////////////////////////////////////////////
void jamBufferReadByte8(JamBuffer *buffer, void *data);

/// \brief Reads a variable amount of data from a buffer into data
void jamBufferReadByteX(JamBuffer *buffer, void *data, uint32 size);

#ifdef __cplusplus
}
#endif
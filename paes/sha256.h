/* This source code is taken, slightly modified, from the GnuPG source code.
 * It's distributed under the terms of the GPL license, version 2 or later.
 *
 * This is the original copyright note:
 *
 *     Copyright (C) 2003 Free Software Foundation, Inc.
 */

#ifndef __SHA256_H__
#define __SHA256_H__ 1

/**
 * \file sha256.h
 *
 * This file contains the prototypes of the functions to be used for SHA256
 * hashing; such functions are taken from the GnuPG project source and are
 * under the terms of GPL 2 or later versions. Their copyright is the following:
 * <b>Copyright (C) 2003 Free Software Foundation, Inc.</b>
 */

#include <sys/types.h>

//! Represents a SHA256 computation context.
typedef struct {
	u_int32_t h0, h1, h2, h3, h4, h5, h6, h7;
	u_int32_t nblocks;
	unsigned char buf[64];
	int count;
} SHA256_CONTEXT;

/**
 * Initializes the SHA256 context; it must be called before doing any hashing.
 * \param hd the SHA256 context to be initialized
 */
void sha256_init(SHA256_CONTEXT * hd);

/**
 * Updates the message digest with the contents of inbuf
 * \param hd the SHA256 context
 * \param inbuf the input buffer
 * \param inlen the input buffer length
 */
void sha256_write(SHA256_CONTEXT * hd, unsigned char *inbuf, size_t inlen);

/**
 * Terminates the computation of the hash associated to the given context.
 * The handle is prepared for a new cycle, but adding unsigned chars to the
 * context will the destroy the returned buffer.
 * \note It must be called after \ref sha256_init and \ref sha256_write.
 * \param hd the SHA256 context
 */
void sha256_final(SHA256_CONTEXT * hd);

/**
 * Reads the content of the hash computed with the given context.
 * \param hd the SHA256 context
 */
unsigned char *sha256_read(SHA256_CONTEXT * hd);

#endif

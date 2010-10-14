/*
    PAES - Parallel AES for CPUs and GPUs
    Copyright (C) 2009  Paolo Bernardi <paolo.bernardi@gmx.it>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 2 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/**
 * \file paes.cl
 * This is where business happens; this code is compiled and executed on CPU or GPU.
 */

/* It seems that OpenCL's preprocessor doesn't play well with #includes,
   but the following include directive works nontheless because this file is 
   explicitly preprocessed in the Makefile via the classic c preprocessor (cpp). */
#include "paes_constants_and_datatypes.h"

__constant const uchar sbox_encrypt[AES_SBOX_SIZE] = AES_SBOX_ENCRYPT;
__constant const uchar sbox_decrypt[AES_SBOX_SIZE] = AES_SBOX_DECRYPT;
__constant const uchar logtable[AES_SBOX_SIZE] = AES_LOGTABLE;
__constant const uchar alogtable[AES_SBOX_SIZE] = AES_ALOGTABLE;

void sub_bytes(size_t block, __global uchar16 * buffer, __constant const uchar * sbox)
{
	buffer[block] = (uchar16) (sbox[buffer[block][0]],
				               sbox[buffer[block][1]],
				               sbox[buffer[block][2]],
				               sbox[buffer[block][3]],
				               sbox[buffer[block][4]],
				               sbox[buffer[block][5]],
				               sbox[buffer[block][6]], 
				               sbox[buffer[block][7]], 
				               sbox[buffer[block][8]], 
				               sbox[buffer[block][9]], 
				               sbox[buffer[block][10]], 
				               sbox[buffer[block][11]], 
				               sbox[buffer[block][12]], 
				               sbox[buffer[block][13]], 
				               sbox[buffer[block][14]], 
				               sbox[buffer[block][15]]);
}

void shift_rows(size_t block, __global uchar16 * buffer)
{
	uchar temp;

	// Rotate first row 1 columns to left   
	temp = buffer[block][4];
	buffer[block][4] = buffer[block][5];
	buffer[block][5] = buffer[block][6];
	buffer[block][6] = buffer[block][7];
	buffer[block][7] = temp;

	// Rotate second row 2 columns to left  
	temp = buffer[block][8];
	buffer[block][8] = buffer[block][10];
	buffer[block][10] = temp;

	temp = buffer[block][9];
	buffer[block][9] = buffer[block][11];
	buffer[block][11] = temp;

	// Rotate third row 3 columns to left
	temp = buffer[block][12];
	buffer[block][12] = buffer[block][15];
	buffer[block][15] = buffer[block][14];
	buffer[block][14] = buffer[block][13];
	buffer[block][13] = temp;
}

void inv_shift_rows(size_t block, __global uchar16 * buffer)
{
	uchar temp;

	// Rotate first row 1 columns to right  
	temp = buffer[block][7];
	buffer[block][7] = buffer[block][6];
	buffer[block][6] = buffer[block][5];
	buffer[block][5] = buffer[block][4];
	buffer[block][4] = temp;

	// Rotate second row 2 columns to right
	temp = buffer[block][8];
	buffer[block][8] = buffer[block][10];
	buffer[block][10] = temp;

	temp = buffer[block][9];
	buffer[block][9] = buffer[block][11];
	buffer[block][11] = temp;

	// Rotate third row 3 columns to right
	temp = buffer[block][12];
	buffer[block][12] = buffer[block][13];
	buffer[block][13] = buffer[block][14];
	buffer[block][14] = buffer[block][15];
	buffer[block][15] = temp;
}

#define GF2M(k, b) ((b) == 0 ? 0 : (alogtable[(logtable[(k)] + logtable[(b)]) % 255]))

void mix_columns(size_t block, __global uchar16 * m)
{
	m[block] = (uchar16) (GF2M(2, m[block][0]) ^ GF2M(3, m[block][4]) ^ m[block][8] ^ m[block][12],
			              GF2M(2, m[block][1]) ^ GF2M(3, m[block][5]) ^ m[block][9] ^ m[block][13],
			              GF2M(2, m[block][2]) ^ GF2M(3, m[block][6]) ^ m[block][10] ^ m[block][14],
			              GF2M(2, m[block][3]) ^ GF2M(3, m[block][7]) ^ m[block][11] ^ m[block][15],
			              GF2M(2, m[block][4]) ^ GF2M(3, m[block][8]) ^ m[block][12] ^ m[block][0],
			              GF2M(2, m[block][5]) ^ GF2M(3, m[block][9]) ^ m[block][13] ^ m[block][1],
			              GF2M(2, m[block][6]) ^ GF2M(3, m[block][10]) ^ m[block][14] ^ m[block][2],
			              GF2M(2, m[block][7]) ^ GF2M(3, m[block][11]) ^ m[block][15] ^ m[block][3],
			              GF2M(2, m[block][8]) ^ GF2M(3, m[block][12]) ^ m[block][0] ^ m[block][4],
			              GF2M(2, m[block][9]) ^ GF2M(3, m[block][13]) ^ m[block][1] ^ m[block][5],
			              GF2M(2, m[block][10]) ^ GF2M(3, m[block][14]) ^ m[block][2] ^ m[block][6],
			              GF2M(2, m[block][11]) ^ GF2M(3, m[block][15]) ^ m[block][3] ^ m[block][7],
			              GF2M(2, m[block][12]) ^ GF2M(3, m[block][0]) ^ m[block][4] ^ m[block][8],
			              GF2M(2, m[block][13]) ^ GF2M(3, m[block][1]) ^ m[block][5] ^ m[block][9], 
			              GF2M(2, m[block][14]) ^ GF2M(3, m[block][2]) ^ m[block][6] ^ m[block][10], 
			              GF2M(2, m[block][15]) ^ GF2M(3, m[block][3]) ^ m[block][7] ^ m[block][11]);
}

void inv_mix_columns(size_t block, __global uchar16 * m)
{
	m[block] = (uchar16) (GF2M(0xe, m[block][0]) ^ GF2M(0xb, m[block][4]) ^ GF2M(0xd, m[block][8]) ^ GF2M(0x9, m[block][12]),
			              GF2M(0xe, m[block][1]) ^ GF2M(0xb, m[block][5]) ^ GF2M(0xd, m[block][9]) ^ GF2M(0x9, m[block][13]),
			              GF2M(0xe, m[block][2]) ^ GF2M(0xb, m[block][6]) ^ GF2M(0xd, m[block][10]) ^ GF2M(0x9, m[block][14]),
			              GF2M(0xe, m[block][3]) ^ GF2M(0xb, m[block][7]) ^ GF2M(0xd, m[block][11]) ^ GF2M(0x9, m[block][15]),
			              GF2M(0xe, m[block][4]) ^ GF2M(0xb, m[block][8]) ^ GF2M(0xd, m[block][12]) ^ GF2M(0x9, m[block][0]),
			              GF2M(0xe, m[block][5]) ^ GF2M(0xb, m[block][9]) ^ GF2M(0xd, m[block][13]) ^ GF2M(0x9, m[block][1]),
			              GF2M(0xe, m[block][6]) ^ GF2M(0xb, m[block][10]) ^ GF2M(0xd, m[block][14]) ^ GF2M(0x9, m[block][2]),
			              GF2M(0xe, m[block][7]) ^ GF2M(0xb, m[block][11]) ^ GF2M(0xd, m[block][15]) ^ GF2M(0x9, m[block][3]),
			              GF2M(0xe, m[block][8]) ^ GF2M(0xb, m[block][12]) ^ GF2M(0xd, m[block][0]) ^ GF2M(0x9, m[block][4]),
			              GF2M(0xe, m[block][9]) ^ GF2M(0xb, m[block][13]) ^ GF2M(0xd, m[block][1]) ^ GF2M(0x9, m[block][5]),
			              GF2M(0xe, m[block][10]) ^ GF2M(0xb, m[block][14]) ^ GF2M(0xd, m[block][2]) ^ GF2M(0x9, m[block][6]),
			              GF2M(0xe, m[block][11]) ^ GF2M(0xb, m[block][15]) ^ GF2M(0xd, m[block][3]) ^ GF2M(0x9, m[block][7]),
			              GF2M(0xe, m[block][12]) ^ GF2M(0xb, m[block][0]) ^ GF2M(0xd, m[block][4]) ^ GF2M(0x9, m[block][8]),
			              GF2M(0xe, m[block][13]) ^ GF2M(0xb, m[block][1]) ^ GF2M(0xd, m[block][5]) ^ GF2M(0x9, m[block][9]),
			              GF2M(0xe, m[block][14]) ^ GF2M(0xb, m[block][2]) ^ GF2M(0xd, m[block][6]) ^ GF2M(0x9, m[block][10]), 
			              GF2M(0xe, m[block][15]) ^ GF2M(0xb, m[block][3]) ^ GF2M(0xd, m[block][7]) ^ GF2M(0x9, m[block][11]));
}

/** 
 * OpenCL kernel that does a single AES round.
 * \param buffer the input/output buffer
 * \param blocks the number of blocks contained in the buffer
 * \param mode one between AES_MODE_ENCRYPT and AES_MODE_DECRYPT
 * \param round_key the AES round keys
 * \param rounds the number of rounds
 * \param round the AES round to do
 */
__kernel __attribute__ ((vec_type_hint(uchar)))
void kernel_aes(__global uchar16 * buffer, const ulong blocks, const uint mode, __constant const uchar16 * round_key, const uint rounds, const uint round)
{
	size_t global_work_size = get_global_size(0);
	size_t global_id = get_global_id(0);
	/* If there are more work items than blocks, each work items takes AT MOST
	   1 block to process, otherwise it could be several blocks per work item. */
	size_t blocks_per_work_item = global_work_size < blocks ? blocks / global_work_size : 1;
	size_t reminder = global_work_size < blocks ? blocks % global_work_size : 0;
	size_t from_block = global_id * blocks_per_work_item;
	if (global_id < reminder)
		from_block += global_id;
	else
		from_block += reminder;

	/* If the work item should start working from a block outside the input data
	   boundaries, it shall do nothing. */
	if (from_block < blocks) {
		/* The last work item will process the input data from its start block to
		   the very last block. The other work items will just do blocks_per_work_item
		   blocks.
		   Each work item will process any block b such as from_block <= b < to_block. */
		//~ size_t to_block = global_id == global_work_size - 1 ? blocks : from_block + blocks_per_work_item;
		size_t to_block = from_block + blocks_per_work_item;
		if (global_id < reminder)
			to_block += 1;
		switch (mode) {
		case AES_MODE_ENCRYPT:
			{
				for (size_t b = from_block; b < to_block; ++b) {
#ifdef SHIFT_ROWS
					shift_rows(b, buffer);
#elif defined(MIX_COLUMNS)
					mix_columns(b, buffer);
#elif defined(ADD_ROUND_KEY)
					buffer[b] ^= round_key[rounds];
#elif defined(SUB_BYTES)
					sub_bytes(b, buffer, sbox_encrypt);
#else
					if (round == 0) {
						buffer[b] ^= round_key[0];
					} else if (round == rounds) {
						sub_bytes(b, buffer, sbox_encrypt);
						shift_rows(b, buffer);
						buffer[b] ^= round_key[rounds];
					} else {
						sub_bytes(b, buffer, sbox_encrypt);
						shift_rows(b, buffer);
						mix_columns(b, buffer);
						buffer[b] ^= round_key[round];
					}
#endif
				}
				break;
			}
		case AES_MODE_DECRYPT:
			{
				for (size_t b = from_block; b < to_block; ++b) {
#ifdef SHIFT_ROWS
					inv_shift_rows(b, buffer);
#elif defined(MIX_COLUMNS)
					inv_mix_columns(b, buffer);
#elif defined(ADD_ROUND_KEY)
					buffer[b] ^= round_key[rounds];
#elif defined(SUB_BYTES)
					sub_bytes(b, buffer, sbox_decrypt);
#else
					if (round == 0) {
						buffer[b] ^= round_key[rounds];
					} else if (round == rounds) {
						inv_shift_rows(b, buffer);
						sub_bytes(b, buffer, sbox_decrypt);
						buffer[b] ^= round_key[0];
					} else {
						inv_shift_rows(b, buffer);
						sub_bytes(b, buffer, sbox_decrypt);
						buffer[b] ^= round_key[rounds - round];
						inv_mix_columns(b, buffer);
					}
#endif
				}
				break;
			}
		}
	}
}

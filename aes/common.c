//I/O library
#include <stdio.h>
//Needed by fstat
#include <sys/stat.h>
//Needed by close()
#include <unistd.h>
//Needed by mmap
#include <sys/mman.h>
//Needed by open()
#include <fcntl.h>
//Needed by strcat()
#include <string.h>
//Needed by timeval
#include <sys/time.h>

#include "common.h"
#include "crypter.h"

// The number of rounds in AES Cipher. It is simply initiated to zero. The actual value is recieved in the program.
int nr = 0;

// The number of 32 bit words in the key. It is simply initiated to zero. The actual value is recieved in the program.
int nk = 0;

// The round constant word array, rcon[i], contains the values given by 
// x to th e power (i-1) being powers of x (x is denoted as {02}) in the field GF(28)
// Note that i starts at 1, not 0).
int rcon[255] = {
	0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
	0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
	0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
	0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
	0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
	0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
	0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
	0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
	0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
	0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
	0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
	0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
	0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
	0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
	0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
	0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb
};


// The array that stores the round keys.
unsigned char round_key[240];

// The Key input to the AES Program
unsigned char *key;

//Global variables to manage files
int fdi, fdo;
char *filename;
char *output_filename;
struct stat status_buf;
int file_size;

// input_file_buffer - it is the array that holds the input file to be encrypted.
// output_file_buffer - it is the array that holds the output file encrypted.
// state - the array that holds the intermediate results during encryption.
unsigned char *input_file_buffer;
unsigned char temp_buffer[4][4];
unsigned char *output_file_buffer;
unsigned char state[4][4];

sint64 now()
{
	struct timeval t;
	gettimeofday(&t, 0);
	sint64 us;
	us = (sint64) t.tv_sec;
	us = us * 1000000;
	us += t.tv_usec;
	return us;
}


// This function produces nb(nr+1) round keys. The round keys are used in each round to encrypt the states.
void key_expansion()
{
	int i, j;
	unsigned char temp[4], k;
	
	// The first round key is the key itself.
	for (i = 0; i < nk; i++) {
		round_key[i * 4] = key[i * 4];
		round_key[i * 4 + 1] = key[i * 4 + 1];
		round_key[i * 4 + 2] = key[i * 4 + 2];
		round_key[i * 4 + 3] = key[i * 4 + 3];
	}

	// All other round keys are found from the previous round keys.
	while (i < (nb * (nr + 1))) {
		for (j = 0; j < 4; j++) {
			temp[j] = round_key[(i - 1) * 4 + j];
		}
		if (i % nk == 0) {
			// This function rotates the 4 bytes in a word to the left once.
			// [a0,a1,a2,a3] becomes [a1,a2,a3,a0]

			// Function RotWord()
			{
				k = temp[0];
				temp[0] = temp[1];
				temp[1] = temp[2];
				temp[2] = temp[3];
				temp[3] = k;
			}

			// SubWord() is a function that takes a four-byte input word and 
			// applies the S-box to each of the four bytes to produce an output word.

			// Function Subword()
			{
				temp[0] = get_sbox_value(temp[0]);
				temp[1] = get_sbox_value(temp[1]);
				temp[2] = get_sbox_value(temp[2]);
				temp[3] = get_sbox_value(temp[3]);
			}

			temp[0] = temp[0] ^ rcon[i / nk];

		} else if (nk > 6 && i % nk == 4) {
			// Function Subword()
			{
				temp[0] = get_sbox_value(temp[0]);
				temp[1] = get_sbox_value(temp[1]);
				temp[2] = get_sbox_value(temp[2]);
				temp[3] = get_sbox_value(temp[3]);
			}
		}

		round_key[i * 4 + 0] = round_key[(i - nk) * 4 + 0] ^ temp[0];
		round_key[i * 4 + 1] = round_key[(i - nk) * 4 + 1] ^ temp[1];
		round_key[i * 4 + 2] = round_key[(i - nk) * 4 + 2] ^ temp[2];
		round_key[i * 4 + 3] = round_key[(i - nk) * 4 + 3] ^ temp[3];
		i++;
	}
}

// This function adds the round key to state.
// The round key is added to the state by an XOR function.
void add_round_key(int round)
{
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			state[i][j] ^= round_key[round * nb * 4 + i * nb + j];
		}
	}
}


//Functions to manage files

bool open_files(char *name_of_file)
{
	fdi = open(name_of_file, O_RDONLY, 0);
	fdo = open(output_filename, O_RDWR | O_CREAT | O_TRUNC, 0000770);
	if (fdi >= 0 && fdo >= 0) {
		return true;
	}
	return false;
}

bool close_file(int file_descriptor)
{
	if (close(file_descriptor) == 0)
		return true;
	return false;
}

bool alloc_files()
{
	fstat(fdi, &status_buf);
	file_size = status_buf.st_size;

	if ((input_file_buffer = (unsigned char *) mmap(0, file_size, PROT_READ, MAP_SHARED, fdi, 0)) < 0) {
		printf("File doesn't mapped in reading mode\n");
		return false;
	}

	if ((output_file_buffer = (unsigned char *) mmap(0, file_size, PROT_WRITE | PROT_READ, MAP_SHARED, fdo, 0)) < 0) {
		printf("File doesn't mapped in writing mode\n");
		return false;
	}

	/* go to the location corresponding to the last byte */
	if (lseek(fdo, file_size - 1, SEEK_SET) == -1) {
		printf("lseek error");
		return false;
	}

	/* write a dummy byte at the last location */
	if (write(fdo, "", 1) != 1) {
		printf("Write error");
		return false;
	}

	return true;
}

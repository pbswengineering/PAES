#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sha256.h"
#include "common.h"
#include "aes.h"
#include "crypter.h"
#include "decrypter.h"

sint64 start_time;
float elapsed_time;

unsigned char *hash_password(char *password, size_t size)
{
	unsigned char *hash = (unsigned char *) malloc(size * sizeof(unsigned char));

	SHA256_CONTEXT context;
	sha256_init(&context);
	sha256_write(&context, (unsigned char *) password, strlen(password));
	sha256_final(&context);

	memcpy(hash, sha256_read(&context), size);
	

	return hash;
}

int main(int argc, char *argv[])
{
	if (argc < 5) {
		printf("Usage: claes <encrypt|decrypt> <input_file> <output_file> <key_size> <password>\n");
		return 0;
	}

	char *action = argv[1];
	filename = argv[2];
	output_filename = argv[3];

	if (strcmp(action, "encrypt") != 0 && strcmp(action, "decrypt") != 0) {
		printf("Specify the right action: crypt or decrypt\n");
		printf("Usage: claes <crypt|decrypt> file\n");
		return 0;
	}

	int key_bits = nr = atoi(argv[4]);

	nk = nr / 32;
	nr = nk + 6;

	char *password = argv[5];
	printf("The password used to encrypt \"%s\" file is '%s'\n", filename, password);
	key = hash_password(password, nk * 4);
	
	printf("password (%d) ", (int)strlen(password));
	for (size_t i = 0; i < strlen(password); ++i)
		printf("%X ", (unsigned char)password[i]);
	printf("\n");
	printf("password hash (%u) ", nk * 4);
	for (size_t i = 0; i < nk * 4; ++i)
		printf("%X ", key[i]);
	printf("\n");
	
	if (!open_files(filename)) {
		printf("File doesn't exist\n");
		return 0;
	}

	alloc_files();

	key_expansion();

	int offset = 0;
	start_time = now();
	for (int k = 0; k < file_size; k++) {
		int pos = k % 16;
		int i = pos / 4;
		int j = pos % 4;
		offset = k - pos;
		temp_buffer[i][j] = input_file_buffer[k];

		if (pos == 15) {
			if (strcmp(action, "encrypt") == 0) {
				cipher(offset);
			} else {
				inv_cipher(offset);
			}

		}
	}
	elapsed_time = (now() - start_time) / 1000.0;

	if (strcmp(action, "encrypt") == 0) {
		printf("Encrypted file written in \"%s\"\n", output_filename);
	} else {
		printf("Decrypted file written in \"%s\"\n", output_filename);
	}

	printf("--\n");
	printf("### Time of computation %f ms ###\n", elapsed_time);

	close_file(fdi);
	close_file(fdo);

	return 0;
}

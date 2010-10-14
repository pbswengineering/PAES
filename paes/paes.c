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
 * \file paes.c
 *
 * This is the main program; basically it contains only the \ref main function
 * and some command line parsing code.
 * Every function that's not necessarily tied with the user interface is defined
 * in the \ref paes_functions.h file.
 */

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "paes_constants_and_datatypes.h"
#include "paes_functions.h"
#include "sha256.h"

/**
 * The default key size in bits, if not specified by the user
 */
const unsigned short default_key_size_bits = 128;

/**
 * Shows some short program usage instructions and exits
 * \param argv the value of command line arguments, including the executable file itself
 */
void show_help(char *argv[])
{
	printf("\nUsage: %s -i INPUT -o OUTPUT -m MODE [-k KEY_SIZE] [-p PASSWD] [-d DEV] [-g GSIZE] [-l LSIZE]\n\n", argv[0]);
	printf("  -i INPUT         the input file\n");
	printf("  -o OUTPUT        the output file\n");
	printf("  -m MODE          MODE can be encrypt or decrypt\n");
	printf("  -k KEY_SIZE      the key size can be 128, 192 or 256 (default is %d)\n", default_key_size_bits);
	printf("  -p PASSWD        the password; if unspecified the user will be asked to type it\n");
	printf("  -d DEV           DEV can be cpu or gpu (default is %s)\n", get_opencl_device_name(DEFAULT_DEVICE));
	printf("  -g GSIZE         the OpenCL global work size (default is decided by OpenCL)\n");
	printf("  -l LSIZE         the OpenCL local work size (default is %u)\n", (unsigned) OPENCL_DEFAULT_LOCAL_SIZE);
	printf("\n");
	exit(EXIT_SUCCESS);
}

/**
 * Parses the command line options and uses them to set some variables
 * \param argc the number of command line arguments, including the executable file itself
 * \param argv the value of command line arguments, including the executable file itself
 * \param input_file_name the pointer to the string where the input file name specified by the user will be stored
 * \param output_file_name the pointer to the string where the output file name specified by the user will be stored
 * \param mode the pointer to the AES mode (keeps track if the task will be to encrypt or to decrypt)
 * \param key_size_bits the pointer to the key size value, in bits
 * \param password the pointer to the password string
 * \param device the pointer to the OpenCL device to be used (cpu or gpu)
 */
void parse_command_line(int argc, char *argv[], char **input_file_name, char **output_file_name, aes_mode * mode, unsigned short *key_size_bits, char **password, opencl_device * device)
{
	/* If the user called the program with no arguments, he has no clue and 
	   needs some tutoring. */
	if (argc == 1)
		show_help(argv);

	int c;

	// Default values
	*key_size_bits = default_key_size_bits;
	*password = NULL;
	*device = DEFAULT_DEVICE;
	*mode = AES_MODE_NONE;

	do {
		c = getopt(argc, argv, "hi:o:m:k:p:d:g:l:");
		switch (c) {
		case 'i':
			*input_file_name = (char *) malloc(sizeof(char) * strlen(optarg) + 1);
			strcpy(*input_file_name, optarg);
			break;
		case 'o':
			*output_file_name = (char *) malloc(sizeof(char) * strlen(optarg) + 1);
			strcpy(*output_file_name, optarg);
			break;
		case 'd':
			if (strcmp(optarg, "cpu") == 0)
				*device = OPENCL_DEVICE_CPU;
			else if (strcmp(optarg, "gpu") == 0)
				*device = OPENCL_DEVICE_GPU;
			else
				*device = OPENCL_DEVICE_NONE;
			break;
		case 'k':
			*key_size_bits = atoi(optarg);
			break;
		case 'p':
			*password = (char *) malloc(sizeof(char) * strlen(optarg) + 1);
			strcpy(*password, optarg);
			break;
		case 'm':
			if (strcmp(optarg, "encrypt") == 0)
				*mode = AES_MODE_ENCRYPT;
			else if (strcmp(optarg, "decrypt") == 0)
				*mode = AES_MODE_DECRYPT;
			else
				*mode = AES_MODE_NONE;
			break;
		case 'h':
			show_help(argv);
		}
	} while (c != -1);
}

/**
 * Checks if the command line arguments are valid.
 * It doesn't include the I/O files because they'll be checked later in the program, when they'll be used.
 * \param mode the AES mode (keeps track if the task will be to encrypt or to decrypt)
 * \param key_size_bits the key size
 * \param device the OpenCL device to be used (cpu or gpu)
 */
void check_arguments(aes_mode mode, unsigned short key_size_bits, opencl_device device)
{
	if (mode == AES_MODE_NONE) {
		fprintf(stderr, "ERROR: wrong AES mode, it should be encrypt or decrypt.\n");
		exit(EXIT_FAILURE);
	}

	if (key_size_bits != 128 && key_size_bits != 192 && key_size_bits != 256) {
		fprintf(stderr, "ERROR: wrong key size, it should be 128, 192 or 256.\n");
		exit(EXIT_FAILURE);
	}

	if (device == OPENCL_DEVICE_NONE) {
		fprintf(stderr, "ERROR: wrong OpenCL device, it should be cpu or gpu.\n");
		exit(EXIT_FAILURE);
	}
}

/** 
 * Returns an hashed version of the given password, truncard to size bytes.
 * \param password the password to be hashed
 * \param size the size of the hashed password
 * \return the hashed password
 */
cl_uchar *hash_password(char *password, size_t size)
{
	cl_uchar *hash = (cl_uchar *) malloc(size * sizeof(cl_uchar));

	SHA256_CONTEXT context;
	sha256_init(&context);
	sha256_write(&context, (unsigned char *) password, strlen(password));
	sha256_final(&context);

	memcpy(hash, sha256_read(&context), size);

	return hash;
}

/** 
 * The main program.
 * \param argc the number of command line arguments (the first is the executable file's name)
 * \param argv the value of command line arguments (the first is the executable file's name)
 */
int main(int argc, char *argv[])
{
	char *input_file_name = NULL, *output_file_name = NULL;
	aes_mode mode;
	unsigned short key_size_bits;
	char *password = NULL;
	cl_uchar *password_hash = NULL;
	opencl_device device;
	cl_uchar *buffer = NULL;

	printf("\n\n-------- PAES --------\n\n\n");

	parse_command_line(argc, argv, &input_file_name, &output_file_name, &mode, &key_size_bits, &password, &device);
	check_arguments(mode, key_size_bits, device);

	size_t size = read_file(input_file_name, &buffer);

	if (password == NULL) {
		char *getpass(const char *prompt);
		password = getpass("\nPlease type the password: ");
	}

	password_hash = hash_password(password, key_size_bits / 8);

	printf("PARAMETERS:\n");
	printf("   Input file: %s\n", input_file_name);
	printf("   Output file: %s\n", output_file_name);
	printf("   AES mode: %s\n", get_aes_mode_name(mode));
	printf("   Key size: %u\n", key_size_bits);
	printf("   Device: %s\n", get_opencl_device_name(device));
	printf("   File size: %u bytes\n", (unsigned) size);
	printf("\n\n");

	if (apply_aes(buffer, size, device, mode, password_hash, key_size_bits) != -1)
		write_file(output_file_name, buffer, size);

	if (buffer)
		free(buffer);
	if (input_file_name)
		free(input_file_name);
	if (output_file_name)
		free(output_file_name);
	if (password)
		free(password);
	if (password_hash)
		free(password_hash);

	printf("\n\n----- It ends here... -----\n\n\n");

	return EXIT_SUCCESS;
}

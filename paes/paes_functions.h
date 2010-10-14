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

#ifndef __PAES_FUNCTIONS_H__
#define __PAES_FUNCTIONS_H__ 1

/**
 * \file paes_functions.h
 *
 * This file contains every function of the host part of PAES that aren't
 * necessarily tied with the user interface.
 */

#include <CL/cl.h>
#include "paes_constants_and_datatypes.h"

/**************************** MISCELLANEOUS FUNCTIONS ****************************/

/** 
 * Allocates enough space for the buffer and puts the file's content into it.
 * \param file_name the name of the file to read
 * \param buffer the pointer to the buffer that will contain the file's content
 * \return the file size
 */
size_t read_file(char *file_name, unsigned char **buffer);

/** 
 * Writes the buffer content into the specified file.
 * \param file_name the name of the file that will be written
 * \param buffer the buffer that contains the data that will be written to the file
 * \param size the buffer's size
 */
void write_file(char *file_name, unsigned char *buffer, size_t size);

/**************************** AES HOST FUNCTIONS ****************************/

/**
 * Returns the string describing the specified AES mode.
 * \param mode one of the AES usage modes (see \ref aes_mode)
 * \return a string describing the specified AES usage mode
 */
char *get_aes_mode_name(aes_mode mode);

/**
 * Returns the string describing the specified OpenCL device.
 * \param device one of the possible OpenCL devices (see \ref opencl_device)
 * \return a string describing the specified OpenCL device
 */
char *get_opencl_device_name(opencl_device device);

/** 
 * Encrypts or decrypts data using AES via OpenCL.
 * \param buffer the data that will be encrypted
 * \param device the OpenCL device type (see \ref opencl_device)
 * \param mode the AES mode (see \ref aes_mode)
 * \param key the AES encryption key
 * \param key_size_bits the encryption key size in bits (128, 192 or 256)
 * \param global_size the OpenCL global work size; if it's OPENCL_DEFAULT_GLOBAL_SIZE is decided by the framework
 * \param local_size the OpenCL local work size
 * \return -1 if something went wrong, 0 otherwise
 */
int apply_aes(cl_uchar * buffer, size_t size, opencl_device device, aes_mode mode, cl_uchar * key, unsigned key_size_bits);

#endif

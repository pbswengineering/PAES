//Size of state matrix
#define MATRIX_SIZE     4*4

// The number of columns comprising a state in AES. This is a constant in AES. Value=4
#define nb      4

// xtime is a macro that finds the product of {02} and the argument to xtime modulo {1b}  
#define xtime(x)   ((x<<1) ^ (((x>>7) & 1) * 0x1b))

// Multiplty is a macro used to multiply numbers in the field GF(2^8)
#define multiply(x,y) (((y & 1) * x) ^ ((y>>1 & 1) * xtime(x)) ^ ((y>>2 & 1) * xtime(xtime(x))) ^ ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^ ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))))

#include <stdbool.h>

typedef long long int sint64;

//Functions
long long int now();
void key_expansion();
void add_round_key(int round);
bool open_files(char *name_of_file);
bool close_file(int file_descriptor);
bool alloc_files();

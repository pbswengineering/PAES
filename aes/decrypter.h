extern int nr;

extern unsigned char *output_file_buffer;

extern unsigned char state[4][4];
extern unsigned char temp_buffer[4][4];

//Functions
int inv_get_sbox_value(int num);
void inv_sub_bytes();
void inv_shift_rows();
void inv_mix_columns();
void inv_cipher(int offset);

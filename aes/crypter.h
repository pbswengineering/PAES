extern int nr;

extern unsigned char *output_file_buffer;

extern unsigned char state[4][4];
extern unsigned char temp_buffer[4][4];

//Functions
int get_sbox_value(int num);
void sub_bytes();
void shift_rows();
void mix_columns();
void cipher(int offset);

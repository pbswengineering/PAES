CC = gcc -std=c99 -g
SOURCES = aes.c common.c crypter.c decrypter.c sha256.c
TARGET = aes

all:
	$(CC) $(DEFINES) $(SOURCES) -o $(TARGET)
	
clean:
	rm -f $(TARGET) *.o *~ dummy.*

#
# Written by Paolo Bernardi <paolo.bernardi@gmx.it>
#

.PHONY: all clean indent doc edit sync test gputest 

CC = gcc
CFLAGS = $(DEFINES) -Wall -Wextra -Werror -pedantic -pedantic-errors -std=c99 -I '$(ATISTREAMSDKROOT)/include/'
LDFLAGS = -L '$(ATISTREAMSDKROOT)/lib/x86_64/' -lOpenCL
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))
TARGET = query
	
all: $(TARGET)

$(TARGET): $(OBJECTS) $(PREPROCESSED_OPENCL_SOURCE)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

$(PREPROCESSED_OPENCL_SOURCE): $(OPENCL_SOURCE)
	cpp $(DEFINES) $(OPENCL_SOURCE) $(PREPROCESSED_OPENCL_SOURCE)

clean:
	rm -fr $(TARGET) *.o *.i *.s *~

indent:
	indent -kr -i8 -l300 *.c

edit:
	scite query.c Makefile &

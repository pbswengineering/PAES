.PHONY: all clean indent doc edit sync test gputest 

CC = gcc
CFLAGS = $(DEFINES) -Wall -Wextra -Werror -pedantic -pedantic-errors -std=c99 -I '$(ATISTREAMSDKROOT)/include/'
LDFLAGS = -L '$(ATISTREAMSDKROOT)/lib/x86_64/' -lOpenCL
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))
OPENCL_SOURCE = paes.cl
PREPROCESSED_OPENCL_SOURCE = preprocessed_$(OPENCL_SOURCE)
TARGET = paes
	
all: $(TARGET)

$(TARGET): $(OBJECTS) $(PREPROCESSED_OPENCL_SOURCE)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

$(PREPROCESSED_OPENCL_SOURCE): $(OPENCL_SOURCE)
	cpp $(DEFINES) $(OPENCL_SOURCE) $(PREPROCESSED_OPENCL_SOURCE)

clean:
	rm -fr $(TARGET) *.o *.i *.s *~ doc/ $(PREPROCESSED_OPENCL_SOURCE)

indent:
	indent -kr -i8 -l300 *.c *.cl *.h

doc:
	doxygen doxygen.cfg
	firefox doc/index.html

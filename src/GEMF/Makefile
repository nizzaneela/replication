 # the compiler: gcc for C program, define as g++ for C++
CC = gcc

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#CFLAGS = -g -Wall
CFLAGS = -Wall
TARGET = gemfc_nrm
all: $(TARGET)

$(TARGET): $(TARGET).c nrm.o para.o common.o
	rm -fr $(TARGET)
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c nrm.o para.o common.o -lm

nrm.o:  nrm.c nrm.h 
	$(CC) $(CFLAGS) -c nrm.c
para.o:  para.c para.h 
	$(CC) $(CFLAGS) -c para.c
common.o:  common.c common.h 
	$(CC) $(CFLAGS) -c common.c

clean:
	rm -rf $(TARGET)
	rm -rf nrm.o
	rm -rf common.o
	rm -rf para.o


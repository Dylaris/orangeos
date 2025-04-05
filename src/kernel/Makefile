# Makefile for kernel

BOOTINC 	= ../boot/include
KERNELINC 	= ../include

ASM 		= nasm
CC 			= gcc
ASMFLAGS 	= -f elf -I $(BOOTINC)
CFLGAS 		= -m32 -I $(KERNELINC) -c -fno-builtin -std=c11

TARGET 	= kernel.o start.o

.PHONY: clean all

all: $(TARGET)

clean:
	rm -f $(TARGET)

kernel.o: kernel.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

start.o: start.c $(KERNELINC)/const.h $(KERNELINC)/type.h $(KERNELINC)/protect.h
	$(CC) $(CFLGAS) -o $@ $<
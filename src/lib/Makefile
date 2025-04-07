# Makefile for lib

KERNELINC 	= ../include

ASM 		= nasm
CC 			= gcc
ASMFLAGS 	= -f elf -I $(KERNELINC)
CFLGAS 		= -m32 -I $(KERNELINC) -c -fno-builtin -fno-stack-protector -std=c11

INC_FILE  	= $(KERNELINC)/const.h $(KERNELINC)/type.h $(KERNELINC)/protect.h $(KERNELINC)/proto.h $(KERNELINC)/global.h
TARGET 		= klib.o kliba.o string.o

.PHONY: clean all

all: $(TARGET)

clean:
	rm -f $(TARGET)

kliba.o: kliba.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

string.o: string.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

klib.o: klib.c $(INC_FILE)
	$(CC) $(CFLGAS) -o $@ $<
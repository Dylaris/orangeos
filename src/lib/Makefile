# Makefile for lib

ASM 		= nasm
ASMFLAGS 	= -f elf

TARGET = kliba.o string.o

.PHONY: clean all

all: $(TARGET)

clean:
	rm -f $(TARGET)

kliba.o: kliba.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

string.o: string.asm
	$(ASM) $(ASMFLAGS) -o $@ $<
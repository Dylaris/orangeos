# Makefile for boot

ASM 		= nasm
ASMFLAGS 	= -I include/

TARGET 		= boot.bin loader.bin

.PHONY: clean all

all: $(TARGET)

clean:
	rm -f $(TARGET)

boot.bin: boot.asm include/load.inc include/fat12hdr.inc
	$(ASM) $(ASMFLAGS) -o $@ $<

loader.bin: loader.asm include/load.inc include/fat12hdr.inc include/pm.inc
	$(ASM) $(ASMFLAGS) -o $@ $<
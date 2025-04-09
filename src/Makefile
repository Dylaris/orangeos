#########################
# Makefile for Orange'S #
#########################

# Entry point of Orange'S
# It must have the same value with 'KernelEntryPointPhyAddr' in load.inc!
ENTRYPOINT 	= 0x30400

# Offset of entry point in kernel file
# It depends on ENTRYPOINT
ENTRYOFFSET =   0x400

# Programs, flags, etc.
ASM      	= nasm
DASM  		= ndisasm
CC      	= gcc
LD      	= ld
ASMBFLAGS 	= -I boot/include/
ASMKFLAGS 	= -I include/ -f elf
CFLAGS  	= -m32 -I include/ -c -fno-builtin -fno-stack-protector -std=c11
LDFLAGS  	= -m elf_i386 -s -Ttext $(ENTRYPOINT)
DASMFLAGS 	= -u -o $(ENTRYPOINT) -e $(ENTRYOFFSET)

# This Program
INC_FILE  		= include/type.h include/const.h include/proc.h include/proto.h \
				  include/global.h
ORANGESBOOT 	= boot/boot.bin boot/loader.bin
ORANGESKERNEL 	= kernel.bin
OBJS  			= kernel/kernel.o kernel/start.o kernel/pic.o kernel/global.o \
				  kernel/trap.o kernel/main.o kernel/clock.o kernel/proc.o kernel/syscall.o \
			      lib/klib.o lib/kliba.o lib/string.o
DASMOUTPUT 		= kernel.bin.asm

# All Phony Targets
.PHONY : everything final image clean realclean disasm all buildimg run

# Default starting position
everything : $(ORANGESBOOT) $(ORANGESKERNEL)

all : realclean everything

run : image
	-bochs -f bochsrc

final : all clean

image : final buildimg

clean :
	rm -f $(OBJS)

realclean :
	rm -f $(OBJS) $(ORANGESBOOT) $(ORANGESKERNEL)

disasm :
	$(DASM) $(DASMFLAGS) $(ORANGESKERNEL) > $(DASMOUTPUT)

buildimg :
	dd if=boot/boot.bin of=a.img bs=512 count=1 conv=notrunc
	sudo mount -o loop a.img /mnt/floppy/
	sudo cp -fv boot/loader.bin /mnt/floppy/
	sudo cp -fv kernel.bin /mnt/floppy
	sudo umount /mnt/floppy

$(ORANGESKERNEL) : $(OBJS)
	$(LD) $(LDFLAGS) -o $(ORANGESKERNEL) $(OBJS)

### BOOT
boot/boot.bin : boot/boot.asm boot/include/load.inc boot/include/fat12hdr.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/loader.bin : boot/loader.asm boot/include/load.inc \
			boot/include/fat12hdr.inc boot/include/pm.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

### KERNEL 
kernel/kernel.o : kernel/kernel.asm include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/syscall.o : kernel/syscall.asm include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/start.o: kernel/start.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

kernel/pic.o : kernel/pic.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

kernel/global.o : kernel/global.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/trap.o : kernel/trap.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

kernel/main.o : kernel/main.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

kernel/clock.o : kernel/clock.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

kernel/proc.o : kernel/proc.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

### LIB
lib/klib.o : lib/klib.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

lib/kliba.o : lib/kliba.asm include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/string.o : lib/string.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

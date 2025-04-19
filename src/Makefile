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
INC_FILE  		= include/type.h include/string.h \
				  include/sys/const.h include/sys/proc.h include/sys/proto.h include/sys/symbol.h \
				  include/sys/keyboard.h include/sys/tty.h include/sys/console.h include/sys/hd.h \
				  include/asm/asm.h include/asm/pic.h include/asm/pit.h include/asm/vga.h include/asm/kb.h \
				  include/asm/hd.h
ORANGESBOOT 	= boot/boot.bin boot/loader.bin
ORANGESKERNEL 	= kernel.bin
OBJS			= kernel/kernel.o kernel/syscall.o kernel/init.o kernel/main.o \
				  kernel/dev/clock.o kernel/dev/keyboard.o kernel/tty.o kernel/console.o \
				  kernel/asm/pic.o kernel/asm/pit.o kernel/symbol.o kernel/trap.o kernel/proc.o \
				  kernel/printf.o kernel/systask.o kernel/dev/hd.o \
				  lib/kliba.o lib/klib.o lib/string.o lib/misc.o

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
kernel/kernel.o : kernel/kernel.asm include/asm/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/syscall.o : kernel/syscall.asm include/asm/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/init.o: kernel/init.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

kernel/asm/pic.o : kernel/asm/pic.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

kernel/asm/pit.o : kernel/asm/pit.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

kernel/symbol.o : kernel/symbol.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

kernel/trap.o : kernel/trap.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

kernel/main.o : kernel/main.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

kernel/dev/clock.o : kernel/dev/clock.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

kernel/proc.o : kernel/proc.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

kernel/dev/keyboard.o : kernel/dev/keyboard.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

kernel/dev/hd.o : kernel/dev/hd.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

kernel/tty.o : kernel/tty.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

kernel/console.o : kernel/console.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

kernel/printf.o : kernel/printf.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

kernel/systask.o : kernel/systask.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

### LIB
lib/klib.o : lib/klib.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

lib/misc.o : lib/misc.c $(INC_FILE)
	$(CC) $(CFLAGS) -o $@ $<

lib/kliba.o : lib/kliba.asm include/asm/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/string.o : lib/string.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

# Makefile for Orange'S

# Entry point of Orange'S
# It must have the same value with 'KernelEntryPointPhyAddr" in load.inc
ENTRYPOINT = 0x30400

# Offset of entry point in kernel file
# It depends on ENTRYPOINT
ENTRYOFFSET = 0x400

# Tool
LD 		= ld
LDFLAGS = -m elf_i386 -s -Ttext $(ENTRYPOINT)

# File
BOOT 	= boot/boot.bin boot/loader.bin
KERNEL 	= kernel.bin
OBJS 	= kernel/kernel.o kernel/start.o lib/kliba.o lib/string.o
IMG 	= os.img

TARGET = $(KERNEL)

# Target
.PHONY: all boot kernel lib buildimg clean

all: boot kernel lib buildimg $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

buildimg: $(BOOT) $(KERNEL)
	dd if=boot/boot.bin of=$(IMG) bs=512 count=1 conv=notrunc
	sudo mount -o loop $(IMG) /mnt/floppy
	sudo cp -fv boot/loader.bin /mnt/floppy
	sudo cp -fv kernel.bin /mnt/floppy
	sudo umount /mnt/floppy

boot:
	make -C boot

kernel:
	make -C kernel

lib:
	make -C lib

clean:
	make -C boot clean
	make -C kernel clean
	make -C lib clean
	rm -f $(TARGET)
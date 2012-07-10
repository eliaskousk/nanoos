include kernel/Makefile
include x86/Makefile
include C++/Makefile
include devices/Makefile
include shell/Makefile

INCLUDE := -I./include 
CFLAGS  := $(INCLUDE) -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -c -g 
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions -fpic 

all: clean $(OBJS)
	@echo " [LD]        kernel"
	@ld -Map nanos.map -T link.ld $(OBJS) -o ./bin/nanos.elf

.c.o:
	@echo " [GCC]		"$@
	@gcc -o $@ $? $(CFLAGS)

.cpp.o:
	@echo " [G++]		"$@
	@gcc -o $@ $? $(CXXFLAGS)

.s.o:
	@echo " [ASM]		"$@
	@nasm -g -felf -o $@ $?

clean:
	@echo "Cleaning object files"
	@rm -f $(OBJS)
	@echo "Removing Kernel"
	@rm -f ./bin/nanos.elf
	@echo "Removing LiveCD if any"
	@rm -f NanoOs.iso
	@echo removing iso folder
	@rm -rf iso

iso: all
	mkdir -p iso/boot/grub
	cp /usr/share/grub/i386-redhat/* ./iso/boot/grub
	cp ./bin/nanos.elf ./iso/boot
	cp menu.cfg ./iso/boot/menu.lst
	mkisofs -U -b boot/grub/iso9660_stage1_5 -no-emul-boot \
		-f -hide boot.catalog -boot-load-size 4 -boot-info-table \
		-o "nanoos.iso" -V "NanOS LiveCD" \
		-quiet iso/ 
iso2: all
	mkdir -p iso/boot/grub
	cp ./bin/nanos.elf ./iso/boot
	cp grub.cfg ./iso/boot/grub
	grub2-mkrescue -o NanoOs.iso iso
floppy: all
		
	@echo "Mounting floppy image "
	@sudo mount /tmp/nanoos.img /mnt -o loop
	@echo "Copying kernle image "
	@sudo cp -ip ./bin/nanos.elf /mnt/boot/nanos.elf
	@sync
	@echo "Unmounting Floppy image"
	@sudo umount /mnt


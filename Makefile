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
	@ld -T link.ld $(OBJS) -o ./bin/nanos.elf
	@echo "Mounting floppy image "
	@sudo mount /tmp/nanoos.img /mnt -o loop
	@echo "Copying kernle image "
	@sudo cp -i ./bin/nanos.elf /mnt/boot/nanos.elf
	@sync
	@echo "Unmounting Floppy image"
	@sudo umount /mnt

.c.o:
	@echo " [GCC]		"$@
	@gcc -o $@ $? $(CFLAGS)

.cpp.o:
	@echo " [G++]		"$@
	@g++ -o $@ $? $(CXXFLAGS)

.s.o:
	@echo " [ASM]		"$@
	@nasm -g -felf -o $@ $?

clean:
	@echo "Cleaning object files"
	@rm -f $(OBJS)
	@echo "Removing Kernel"
	@rm -f ./bin/nanos.elf
	

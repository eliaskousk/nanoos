#!/bin/bash
msg()
{
    echo -e "\033[00;31m $1 \033[0m"
}
err()
{
    echo -e "\033[42;31m $1 \033[0m"
}

echo
err "//////////////////////////////////////////////////////////"
err "// This file is a part of Nanos Copyright (C) 2008, 2009//"
err "// ashok.s.das@gmail.com                                //"
err "//////////////////////////////////////////////////////////"
err "// Build script for building nanos kernel               //"
err "//                                                      //"
err "//////////////////////////////////////////////////////////"
echo
msg "Compiling ........."


g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g video.cpp 
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g kernel.cpp 
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g gdt.cpp
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g idt.cpp
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g string.cpp
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g OStream.cpp 
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g irq.cpp
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g runtime.cpp
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g kheap.cpp
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g timer.cpp
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g kbd.cpp
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g IStream.cpp
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g shell.cpp
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g cmos.cpp
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g multiboot.cpp
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g gcpumore.cpp
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g ide.cpp
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g drive.cpp
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g fatfs.cpp
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g pci.cpp
g++  -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c -g task.cpp

echo 
msg "Assembling asm files using nasm "
echo
nasm -f elf loader.asm -o loader.o
nasm -f elf isr_wrap.asm -o isr_wrap.o
nasm -f elf gcpun.asm -o gcpun.o
msg "removing old kernel"
rm -f ./bin/nanos.elf
echo
msg "Linking kernel... please see map file for details"
echo
ld -T link.ld -Map nano-os.map loader.o runtime.o video.o kheap.o string.o \
	OStream.o multiboot.o isr_wrap.o gdt.o idt.o irq.o kbd.o IStream.o \
	ide.o drive.o cmos.o task.o timer.o fatfs.o pci.o \
	gcpun.o gcpumore.o kernel.o  shell.o \
         -o ./bin/nanos.elf 

msg "removing object files and backup files"


rm -f *.o
rm -f *.*~
rm -f include/*.*~

#echo -e "DONE : Please check bin directory for the resulting Kernel"
if [ -f "bin/nanos.elf" ]; then
msg  "Congrats The Kernel is Successfully created"
else 
    err "Some error somewhere please check the compiler messages"
fi

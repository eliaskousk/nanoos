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


g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c video.cpp 
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c kernel.cpp 
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c gdt.cpp
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c idt.cpp
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c string.cpp
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c OStream.cpp 
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c irq.cpp
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c runtime.cpp
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c kheap.cpp
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c timer.cpp
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c kbd.cpp
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c IStream.cpp
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c shell.cpp
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c cmos.cpp
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c multiboot.cpp
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c gcpumore.cpp
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c ide.cpp
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c drive.cpp
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c fatfs.cpp

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
ld -T link.ld  -Map nano-os.map loader.o kernel.o video.o runtime.o string.o OStream.o kheap.o gdt.o idt.o isr_wrap.o irq.o timer.o kbd.o \
      IStream.o cmos.o multiboot.o gcpun.o gcpumore.o ide.o drive.o fatfs.o shell.o -o ./bin/nanos.elf 

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

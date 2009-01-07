echo -e "//////////////////////////////////////////////////////////"
echo -e "// This file is a part of Nanos Copyright (C) 2008, 2009//"
echo -e "// ashok.s.das@gmail.com                                //"
echo -e "//////////////////////////////////////////////////////////"
echo -e "// Build script for building nanos kernel               //"
echo -e "//                                                      //"
echo -e "//////////////////////////////////////////////////////////"
echo
echo -e "Compiling ........."
echo

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
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c dma.cpp
#g++ -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c fdc.cpp
#g++ -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c bddl.cpp
#g++ -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c floppy.cpp
g++ -O0 -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c ide.cpp

echo 
echo -e "Assembling asm files using nasm"
echo
nasm -f elf loader.asm -o loader.o
nasm -f elf isr_wrap.asm -o isr_wrap.o
nasm -f elf gcpun.asm -o gcpun.o
echo -e "removing old kernel"
rm -f ./bin/nanos.elf
echo
echo -e "Linking kernel... please see map file for details"
echo
ld -T link.ld  -Map nano-os.map loader.o kernel.o video.o runtime.o string.o OStream.o kheap.o gdt.o idt.o isr_wrap.o irq.o timer.o kbd.o \
      IStream.o cmos.o multiboot.o gcpun.o gcpumore.o dma.o ide.o shell.o -o ./bin/nanos.elf 
echo
echo -e "removing object files and backup files"
echo

rm -f *.o
rm -f *.*~
rm -f include/*.*~

echo -e "DONE : Please check bin directory for the resulting Kernel"

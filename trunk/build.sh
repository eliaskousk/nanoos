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

g++ -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c video.cpp 
g++ -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c kernel.cpp 
g++ -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c gdt.cpp
g++ -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c idt.cpp
g++ -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c string.cpp
g++ -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c OStream.cpp 
g++ -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c irq.cpp
g++ -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c runtime.cpp
g++ -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c kheap.cpp
g++ -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c timer.cpp
g++ -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c kbd.cpp
g++ -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c IStream.cpp
g++ -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c shell.cpp
g++ -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c cmos.cpp
g++ -I./include -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -c multiboot.cpp

echo 
echo -e "Assembling asm files using nasm"
echo
nasm -f elf loader.asm -o loader.o
nasm -f elf isr_wrap.asm -o isr_wrap.o
echo -e "removing old kernel"
rm -f ./bin/nanos.elf
echo
echo -e "Linking kernel... please see map file for details"
echo
ld -T link.ld  -Map nano-os.map loader.o kernel.o video.o runtime.o string.o OStream.o kheap.o gdt.o idt.o isr_wrap.o irq.o timer.o kbd.o \
      IStream.o shell.o cmos.o multiboot.o -o ./bin/nanos.elf 
echo
echo -e "removing object files"
echo

rm -f kernel.o
rm -f video.o
rm -f OStream.o
rm -f string.o
rm -f loader.o
rm -f gdt.o
rm -f idt.o
rm -f isr_wrap.o
rm -f irq.o
rm -f kheap.o
rm -f kbd.o
rm -f timer.o
rm -f IStream.o
rm -f shell.o
rm -f cmos.o
rm -f runtime.o
rm -f multiboot.o

rm -f *.*~
echo -e "DONE : Please check bin directory for the resulting Kernel"

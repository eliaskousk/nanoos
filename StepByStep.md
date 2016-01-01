#On this page I will try to Put how I Started and How I approached.

# Introduction #

On the Net there are several OSdev articles. Just Google the right words/phrases you will get thousands of them. The Person I got inspiration is  **Alexei A. Frounze** As he has shown The way how to create a small OS using Turbo-C Well at least I was inspired by his Site.
Next is **Bran's Kernel tutorial** Which I personally recommend and of-course the **James Molloy's** roll your own stuff. Well I of-course can not forget the famous **GeekOs, chris and megatokyo** (now defunct????)

# How It started #
During 1997-98 I stumbled upon few Geeks locally Who are wasting there talents writing Nuisance. They were in need of a guy good in Assembly and C, so some mutual wise friend
recommend my name. Those guy's are pretty junior to me and aged between 18-20. They asked me to help them with fundamentals of Assembly language and how to interface with High-level languages like PASCAL and C. During those days I used _DOS debug.exe_ for writing small utilities and tools(Well it was tedious but interesting). Then I started to describe them The inline-assembly in Turbo-C and Turbo-Pascal( Yes Once Upon a time I was a Pascal Fan). The Interaction between me And That group went well for 1-year or so. After that I left my Native place and moved away.

During those days I stumbled upon Alex's home page and soiled my hands with Protected mode. Then On to the OSDev. The OSDev Using Turbo-C. I modified bits and pices here and there crashed my system few times then I grabbed a copy of BOCHS a beautifull PC-Emulator which saved my PC from rebooting from Floppy disk to try out the modifications and there effects. After a few days I lost Interest in OSDev as my priority changed.
Then comes a period of dark-age for 5-years. At that Time I was Working with WYSE Technology,India. There I had to work on eCos, an embedded OS for a small subsystem. The work involved Modifying several things inside the Kernel. That again kindle the flame in me. But I didn't start. Then I left WYSE and Joined Redhat,India. I Usually being asked about few curious things from Support guys regarding boot process, scheduling, drivers etc...
One fine day I thought Why not write a small Kernel to demonstrate these features?
So I started One OS in C. Then I Decided to Write it in C++ as youngsters comming out of colleges are more aquanted with C++ and java than C.
I took one bare-bone for C++ from OSDEV.org and started populating it. I thought of a name for it as **nanos** stands for _Not An OS_ but There already was a project in the same name. so i changed the name to **nanoos** for _Nano OS_.

# Details #
The **Nanoos** code is not well organised besides spliting directory for header(include)files and source files(The code from 10th June 2010 it is organised in separate folders).
Header files are for C++ headers and Source files are C++ and Assembly(x86) source. I choose **NASM** and G++ under Linux for development as there are plenty of materials available and they are free.
## Boot ##
**Nanoos** Doesn't have a separate boot loder. So **GRUB** is used to boot the **nanoos** kernel. The **nanoos** is compiled as **ELF** binary and **GRUB** supports booting **ELF**.
The first code to start after **GRUB** is the _.asm_ code _loader.asm_. This file Calls our C++ kernel propper by calling kmain() from _kernel.cpp_. From kmain() all other sub-systems are called.
The First to do is Printing a string to the screen. Well It could be done in a line. But we need more thing. So a Video class is Written which will handle text output to the screen.
But a real C++ program uses a _cout_ hence a Ostream is implemented over video class.

## Text output to Screen ##
_See Video.h and Video.cpp for details._
The Assumption we made is CGA here. We detect the CGA type to find if the display hardware is _MONO_ or _COLOUR_
After we detect, we write the character to the specified address and the attribute(if any) to the next address.

## Memory handling ##
As it is intended to be a 32-bit protected mode OS, It should have a GDT(Global Descripter Table).
This GDT decides how the memory will look like in our OS.
Here we will set our _Kernel code_ segment and _kernel data_ segment.
In future when we are ready for the User space we will put _user code_ segment and _user data_ segment.

### Available memory ###
Here we are completely dependant on GRUB. Well we could probe for the memory, But why take pain if somebody else is doing the work for us; is not it.
GRUB gives us _kernel start_, _kernel end_ and _memory end_. using this information
we can safely write a memory manager.

### Memory Manager ###
Memory management in any OS is a big big code and problem. In 32-bit protected mode we are able to access 4 GB code and data; as our GDT sets this up for us.
Now we can go for a paged memory and virtual memory. But laziness is our moto.
instead of virtual memory using paging we are going a strait-forward approach. Only we will map the available memory.
This means from _kernel end_ to _memory end_ will be mapped and and allocated in chunks when **malloc** is called.
**Nanoos** borrowed the code from **Chris gheese** for allocation and de-allocation.
As we are writing a C++ OS, _new_ and _delete_ were used as wrapper over _malloc_ and _free_.
Refer kheap.h kheap.cpp for more details.

## Interrupts and Exceptions handling ##
Exceptions are CPU generated. Examples are Divided by zero, GPF, Double Fault etc...
Interrupts are generated by Hardware or software. Hardware interrupts are generated when a particular hardware needs attaintion. It generates a signal, and the Interrupt controller generates an interrupt(IRQ).
once the interrupt is generated it should be served. The routine serves this is known as **Interrupt Service Routine**.
All Exceptions and IRQs are interrupts hence all of them have there respective ISR.

In **nanoos** we have a framework for **ISRs** and **IRQs** based upon bran's kernel tutorial.
See isr-wrap.asm,IDT.cpp,IRQ.cpp and header files.

## Keyboard input ##
Keyboard is hooked to IRQ1, whenever keyboard needs attaintion, it raises IRQ1, and our _Keyboard handler_
handles it by reading from the keyboard. **cin** is associated with keyboard handler code.
See kbd.cpp and IStream.cpp for more info.
## timer ##
Clock pulses are the life or heart-beat of any digital system. So PC also uses those pulses for it's execution.
Similarly we keep time in the kernel.
See timer.cpp for more info.

## tasking ##
**Nanoos** has very primitive multi tasking support. this is achieved by **CPU context switching**.
basically whenever a timer interrupt is generated we save the _cpu register_ and load next process's register values to the CPU.
UPDATE: We are now switching stack to achieve multi tasking. See the ISR\_wrap.asm, task.cpp for more details. The create\_thread() function is similar to geekos. but the task switching is not. To make things simple we just return the stack\_top of the task, and store it to esp in isr\_wrap.asm. then the irq0 routine restores the coresponding registers. so our new task is now executing.

## driver ##
**ATA driver and FAT-16 drivers** are in alpha stage. Even PCI driver needs to be optimized.
I would greatly appreciate if some one can tune the ATA system (myata.cpp, mydrive.cpp) for me :))

**RTL8139** driver is in pre alpha stage.

**_I would be very happy if some one can contribute._**
## other hardware ##
to be done
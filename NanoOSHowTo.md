#This Page will try to describe how to use and test Nano OS

# Making/compiling #
**downloading and pre requisite**
  1. One PC with Linux, gcc 4.2 and associated package and NASM
  1. SVN client to get the source code
  1. Patience to Tolerate huge number of Compiler Warnings.

**compilation and testing**
  1. Go to downloaded source directory
  1. Execute make
  1. If something is wrong then you are screwed somewhere please check all the messages.
  1. The make file is configured for mounting the /tmp/nanoos.img and copying the kernel to it.
  1. To try out some hard disk image create some hd image using dd;(TEMPORARILY WITHDRAWN)
  1. Get a FreeDos Livecd to boot "qemu -cdrom fdfull.iso -hda <your hd image name> -boot d"
  1. after booting into DOS do fdisk and save
  1. exit FreeDos
  1. boot nano os as "qemu -fda <nanoos fd image> -hda <your hd image name> -boot a"
  1. Now play arround
  1. to check IDE issue "ide" on command prompt
  1. to check if it properly detected FAT16, issue "sysdriveinfo" at command prompt.
Add your content here.


# Details #

Add your content here.  Format your content with:
  * Text in **bold** or _italic_
  * Headings, paragraphs, and lists
  * Automatic links to other wiki pages
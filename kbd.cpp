//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// keyboard implementation                              //
//                                                      //
//////////////////////////////////////////////////////////
/* keyboard handler routine */
#include "kbd.h"
#include "low-io.h"
#include "irq.h"
namespace kbd{

static int shift=0,ctrl=0,alt=0,capslock=0;
static unsigned char kbd_buffer[129];
static int buffend=0;

unsigned short kbdus_unshifted[] =
{
    KEY_UNKNOWN, ASCII_ESC, '1', '2',   /* 0x00 - 0x03 */
    '3', '4', '5', '6',                 /* 0x04 - 0x07 */
    '7', '8', '9', '0',                 /* 0x08 - 0x0B */
    '-', '=', ASCII_BS, '\t',           /* 0x0C - 0x0F */
    'q', 'w', 'e', 'r',                 /* 0x10 - 0x13 */
    't', 'y', 'u', 'i',                 /* 0x14 - 0x17 */
    'o', 'p', '[', ']',                 /* 0x18 - 0x1B */
    '\n', KEY_LCTRL, 'a', 's',          /* 0x1C - 0x1F */
    'd', 'f', 'g', 'h',                 /* 0x20 - 0x23 */
    'j', 'k', 'l', ';',                 /* 0x24 - 0x27 */
    '\'', '`', KEY_LSHIFT, '\\',        /* 0x28 - 0x2B */
    'z', 'x', 'c', 'v',                 /* 0x2C - 0x2F */
    'b', 'n', 'm', ',',                 /* 0x30 - 0x33 */
    '.', '/', KEY_RSHIFT, KEY_PRINTSCRN, /* 0x34 - 0x37 */
    KEY_LALT, ' ', KEY_CAPSLOCK, KEY_F1, /* 0x38 - 0x3B */
    KEY_F2, KEY_F3, KEY_F4, KEY_F5,     /* 0x3C - 0x3F */
    KEY_F6, KEY_F7, KEY_F8, KEY_F9,     /* 0x40 - 0x43 */
    KEY_F10, KEY_NUMLOCK, KEY_SCRLOCK, KEY_KPHOME,  /* 0x44 - 0x47 */
    KEY_KPUP, KEY_KPPGUP, KEY_KPMINUS, KEY_KPLEFT,  /* 0x48 - 0x4B */
    KEY_KPCENTER, KEY_KPRIGHT, KEY_KPPLUS, KEY_KPEND,  /* 0x4C - 0x4F */
    KEY_KPDOWN, KEY_KPPGDN, KEY_KPINSERT, KEY_KPDEL,  /* 0x50 - 0x53 */
    KEY_SYSREQ, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,  /* 0x54 - 0x57 */
};		
#define SCAN_TABLE_SIZE (sizeof(kbdus_unshifted) / sizeof(unsigned char))
unsigned short kbdus_shifted[]={
    KEY_UNKNOWN, ASCII_ESC, '!', '@',   /* 0x00 - 0x03 */
    '#', '$', '%', '^',                 /* 0x04 - 0x07 */
    '&', '*', '(', ')',                 /* 0x08 - 0x0B */
    '_', '+', ASCII_BS, '\t',           /* 0x0C - 0x0F */
    'Q', 'W', 'E', 'R',                 /* 0x10 - 0x13 */
    'T', 'Y', 'U', 'I',                 /* 0x14 - 0x17 */
    'O', 'P', '{', '}',                 /* 0x18 - 0x1B */
    '\n', KEY_LCTRL, 'A', 'S',          /* 0x1C - 0x1F */
    'D', 'F', 'G', 'H',                 /* 0x20 - 0x23 */
    'J', 'K', 'L', ':',                 /* 0x24 - 0x27 */
    '"', '~', KEY_LSHIFT, '|',          /* 0x28 - 0x2B */
    'Z', 'X', 'C', 'V',                 /* 0x2C - 0x2F */
    'B', 'N', 'M', '<',                 /* 0x30 - 0x33 */
    '>', '?', KEY_RSHIFT, KEY_PRINTSCRN, /* 0x34 - 0x37 */
    KEY_LALT, ' ', KEY_CAPSLOCK, KEY_F1, /* 0x38 - 0x3B */
    KEY_F2, KEY_F3, KEY_F4, KEY_F5,     /* 0x3C - 0x3F */
    KEY_F6, KEY_F7, KEY_F8, KEY_F9,     /* 0x40 - 0x43 */
    KEY_F10, KEY_NUMLOCK, KEY_SCRLOCK, KEY_KPHOME,  /* 0x44 - 0x47 */
    KEY_KPUP, KEY_KPPGUP, KEY_KPMINUS, KEY_KPLEFT,  /* 0x48 - 0x4B */
    KEY_KPCENTER, KEY_KPRIGHT, KEY_KPPLUS, KEY_KPEND,  /* 0x4C - 0x4F */
    KEY_KPDOWN, KEY_KPPGDN, KEY_KPINSERT, KEY_KPDEL,  /* 0x50 - 0x53 */
    KEY_SYSREQ, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,  /* 0x54 - 0x57 */
};
unsigned short kbdus_caps[] =
{
    KEY_UNKNOWN, ASCII_ESC, '1', '2',   /* 0x00 - 0x03 */
    '3', '4', '5', '6',                 /* 0x04 - 0x07 */
    '7', '8', '9', '0',                 /* 0x08 - 0x0B */
    '-', '=', ASCII_BS, '\t',           /* 0x0C - 0x0F */
    'Q', 'W', 'E', 'R',                 /* 0x10 - 0x13 */
    'T', 'Y', 'U', 'I',                 /* 0x14 - 0x17 */
    'O', 'P', '[', ']',                 /* 0x18 - 0x1B */
    '\n', KEY_LCTRL, 'A', 'S',          /* 0x1C - 0x1F */
    'D', 'F', 'G', 'H',                 /* 0x20 - 0x23 */
    'J', 'K', 'L', ';',                 /* 0x24 - 0x27 */
    '\'', '`', KEY_LSHIFT, '\\',        /* 0x28 - 0x2B */
    'Z', 'X', 'C', 'V',                 /* 0x2C - 0x2F */
    'B', 'N', 'M', ',',                 /* 0x30 - 0x33 */
    '.', '/', KEY_RSHIFT, KEY_PRINTSCRN, /* 0x34 - 0x37 */
    KEY_LALT, ' ', KEY_CAPSLOCK, KEY_F1, /* 0x38 - 0x3B */
    KEY_F2, KEY_F3, KEY_F4, KEY_F5,     /* 0x3C - 0x3F */
    KEY_F6, KEY_F7, KEY_F8, KEY_F9,     /* 0x40 - 0x43 */
    KEY_F10, KEY_NUMLOCK, KEY_SCRLOCK, KEY_KPHOME,  /* 0x44 - 0x47 */
    KEY_KPUP, KEY_KPPGUP, KEY_KPMINUS, KEY_KPLEFT,  /* 0x48 - 0x4B */
    KEY_KPCENTER, KEY_KPRIGHT, KEY_KPPLUS, KEY_KPEND,  /* 0x4C - 0x4F */
    KEY_KPDOWN, KEY_KPPGDN, KEY_KPINSERT, KEY_KPDEL,  /* 0x50 - 0x53 */
    KEY_SYSREQ, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,  /* 0x54 - 0x57 */
};		


unsigned short scan_key()
{
	static unsigned char code, val;
	
	code = inportb(KB_DATA_PORT);	// Get scan code
	val =  inportb(KB_ACK_PORT);	// Get keyboard acknowledge
	outportb(KB_ACK_PORT, val | 0x80); // Disable bit 7
	outportb(KB_ACK_PORT, val);	// Send that back

	return( code );
}
void kbd_handler(IDT::regs *r)
{
		
	unsigned short scan_code;
	unsigned char ch;
	scan_code=scan_key();
	if (scan_code & 0x80) //keyrelease
	{
		//kprintf("A key is released scancode= %d\n",scan_code);		
		switch(kbdus_unshifted[scan_code-128])
		{
			case KEY_LSHIFT:
			case KEY_RSHIFT:
				shift = 0;
				break;
			case KEY_LCTRL:
			case KEY_RCTRL:
				ctrl = 0;
				break;
			case KEY_LALT:
			case KEY_RALT:
				alt = 0;
				break;
		}
	}
//key is pressed
	else
	{
		if (kbdus_unshifted[scan_code] == KEY_CAPSLOCK)
		{
			if(capslock==0) capslock = 1;
			else capslock=0;
		}
		if ((kbdus_unshifted[scan_code] == KEY_LSHIFT) || (kbdus_unshifted[scan_code] ==KEY_RSHIFT))
		{
			shift = 1;
		}
		if ((kbdus_unshifted[scan_code] == KEY_LCTRL)||(kbdus_unshifted[scan_code] == KEY_RCTRL))
		{
			ctrl = 1;
		}
		if ((kbdus_unshifted[scan_code] ==KEY_LALT) || (kbdus_unshifted[scan_code] ==KEY_RALT))
		{
			alt = 1;
		}
		if(!ctrl || !alt)
		{
			if(shift)
				ch=kbdus_shifted[scan_code];
			else if(capslock)
				ch=kbdus_caps[scan_code];
			else
				ch=kbdus_unshifted[scan_code];
		}
		//cout<<ch;
		if(buffend!=128)
		{
			buffend++;
		}
		kbd_buffer[buffend]=ch;
		
	}	
	//kprintf("key pressed => %u\n",scan_code);
	
}
char getch()
{
	int i = 0;
	while(buffend == 0);

	//disable();		//Disable interrupts while we modify the buffer

	for(; i < buffend; i++)
	{
		kbd_buffer[i] = kbd_buffer[i + 1];
	}
	buffend--;

	//enable();

	return kbd_buffer[0];
}
/*void install_kbd()
{
	init_kbd();	
	irq_install_handler(1,kbd_handler);
}*/
void kbd_wait()
{
	int retries = 1000;

	while( ((inportb(KB_CMD_PORT) & 0x02) == 0x02) )
	{
		
		if( --retries == 0 )
			break;
	}
}
void send_command(unsigned char c)	//Sends a command to the keyboard
{
 	kbd_wait();
 	outportb(KB_CMD_PORT,c);
}
void send_data(unsigned char data) //Sends a command or data to keyboard
{
	kbd_wait();		//MicroDelay((unsigned long) 10);
	outportb(KB_DATA_PORT, data);
}
void set_leds(unsigned char leds)
{
	send_data(0xED);
	send_data(leds);	
	/*kbd_wait();
        outportb(KB_DATA_PORT, 0xED);
        kbd_wait();
        outportb(KB_DATA_PORT, leds);*/
}
void kbd_flush_buff()	//Clears the keyboard buffer (Hardware buffer)
{
	unsigned temp;
	do
	{
		temp = inportb(KB_CMD_PORT);
		if((temp & 0x01) != 0) 
		{
			(void)inportb(KB_DATA_PORT);
			continue;
		}
	} while((temp & 0x02) != 0);
}
void setup()
{
	kbd_flush_buff();	
	send_data(0xFF);		//Restart keyboard
	send_data(0xF4);		//Enables keyboard and Scanning	
	IRQ::install_handler(1,&kbd_handler);
}
}; //end namespace kbd

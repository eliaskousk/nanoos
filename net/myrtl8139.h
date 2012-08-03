#ifndef __MYRTL_H__
#define __MYRTL_H__
#include "pci.h"
#include "low-io.h"
#define TSD0			0x10
#define TSAD0			0x20

#define RTL8139_CMD		0x37
// command register bit values
#define RESET                (1 << 4)
#define RECEIVER_ENABLE      (1 << 3)
#define TRANSMITTER_ENABLE   (1 << 2)
#define BUFFER_IS_EMPTY      (1 << 0)

#define RBSTART			0x30
#define IMR			0x3c
#define ISR			0x3e
#define RXOK			(1<<0)	// Receive OK
#define TXOK			(1<<2)  // tx ok
#define RCR			0x44
#define AAP		(1<<0) // accept all
#define APM		(1<<1) // accept physical match ( meant only for this)
#define AM		(1<<2)	// Accept Multicast packet
#define AB		(1<<3)	// accept broadcast packet
#define AR		(1<<4)	// accept runt
#define AER		(1<<5)	// accept error packet
#define WRAP		(1<<7)	//wrap , if set to 0 the remaider will be written to the begining of rx buffer, if 1 then memory after
				// the buffer so atleast 1.5k bytes be left at the end of rx buffer to accomodate overflow
#define CONFIG1		0x52
//current address of packet read
#define CAPR	0x0038
//media status Register
#define MSR	0x0058
//Basic Mode Control Reg
#define BMCR	0x0062
//basic mode status reg
#define BMSR	0x0064

class rtl8139
{
	private:
		unsigned char mac_id[6];
		void read_mac();	// we read mac id only once to populate mac_id[]
		unsigned short iobase;  // will be supplied by pci subsystem
		unsigned char irq;
	protected:
		static void irq_handler(void *sd);
		struct packet {
			unsigned short status;
			unsigned short size;
		};
		int send_index;
		int received_index;
		unsigned char *rec_buffer;
	public:
		void init(unsigned short base, unsigned char intr);
		void send(void *buffer,int len); // send the buffer of length len to hw
		unsigned short receive(void *buffer);      // read from hw and copy to buffer
		void get_mac(unsigned char *buf); //will copy the mac_id[] to buf
		void show_mac(); // will print the mac 
		void info();
};

#endif


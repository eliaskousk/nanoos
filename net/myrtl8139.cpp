//mrtl
#include "low-io.h"
#include "irq.h"
#include "timer.h"
#include "string.h"
#include "OStream.h"
#include "myrtl8139.h"
#include "utils.h"

using namespace String;

/* Size of the in-memory receive ring. */
#define RX_BUF_LEN_IDX	0			/* 0==8K, 1==16K, 2==32K, 3==64K */
#define RX_BUF_LEN (8192 << RX_BUF_LEN_IDX)
/* Size of the Tx bounce buffers -- must be at least (dev->mtu+14+4). */
#define TX_BUF_SIZE	1536
/* The rest of these values should never change. */
#define NUM_TX_DESC	4			/* Number of Tx descriptor registers. */
/* The following settings are log_2(bytes)-4:  0 == 16 bytes .. 6==1024. */
#define RX_FIFO_THRESH	4		/* Rx buffer level before first PCI xfer.  */
#define RX_DMA_BURST	4		/* Maximum PCI burst, '4' is 256 bytes */
#define TX_DMA_BURST	4		/* Calculate as 16<<val. */
#define MAX_DMA_BURST	7
#define TX_FIFO_THRESH 256	/* In bytes, rounded down to 32 byte units. */

rtl8139 *rtl_dev;

// reset the RTL 8139 card
bool rtl_8139_reset(unsigned short base)
{
	unsigned short timeout = 100;
	outportb(base + RTL8139_CMD , RESET); // send reset command
	while(timeout--)			// wait for reset to be completed
	{
		my_timer->sleep(10);
		if(!(inportb(base + RTL8139_CMD) & RESET))
		return true; // return true if we finished before timeout
	}
	return false;
}

void rtl8139::init(unsigned short base, unsigned char intr)
{
	cout<<"Initializing RTL 8139 NIC...";
	iobase = base;
	irq = intr;
	IRQ::install_handler(irq,rtl8139::irq_handler);
	IRQ::enable_irq(irq);
	if(!rtl_8139_reset(iobase))		// reset the RTL 8139 chip
	{
		cout.SetColour(RED,BLACK,0);
		cout<<"RTL 8139 reset failed\n";
		cout.SetColour(WHITE,BLACK,0);
		return;
	}
	//outportb(iobase + CONFIG_9346, 0xC0); // unlock config registers
	outportl(iobase + TCR,0x03000700);
	outportl(iobase + RCR,0x0000070a);
	rec_buffer = NULL;
	rec_buffer = new unsigned char[8192+16]; // get 8kb+16 byte receive buffer
	tx_buffer = new unsigned char [TX_BUF_SIZE * NUM_TX_DESC];
	if((rec_buffer == NULL) ||(tx_buffer == NULL))
	{
		cout.SetColour(RED,BLACK,0);		
		cout<<"No memory for RX/TX buffer\n Exiting\n";
		cout.SetColour(WHITE,BLACK,0); 
		if(rec_buffer) delete rec_buffer;
		if(tx_buffer) delete tx_buffer;
		return;
	}
	memset(rec_buffer,0,8196+16);
	memset(tx_buffer,0,TX_BUF_SIZE * NUM_TX_DESC);
	outportb(iobase + RTL8139_CMD, RECEIVER_ENABLE | TRANSMITTER_ENABLE); // enable the tx and rx mechanism
	outportl(iobase + RCR, (MAX_DMA_BURST<<8)|( AAP | APM | AM | AB | AR | WRAP));
	outportl(iobase + TCR, (MAX_DMA_BURST<<8)|TCR_IFG_STD);
	outportl(iobase + 0xDA, 0x1FFF); // MAx RX packet size
	outportb(iobase + 0xEC, 0x3B); // MAx TX packet size
	outportw(iobase + ISR, 0x0000);     // enable IRQ
	outportw(iobase + IMR,0xFFFF );//RXOK | TXOK); // generate ieq for rxok and txok
	outportw(iobase + RBSTART, (unsigned int)rec_buffer); // set the rx buffer start address
	received_index = send_index =0;
	//outportl (iobase + RXMISSED, 0x0);
	read_mac(); // read the MAC ID from IDRs
	//outportl(iobase +MAR0,0xffffffff);
	//outportl(iobase +MAR4,0xffffffff);
	//outportb(iobase + HLTCLK, 'R');
	//outportb(iobase + CONFIG_9346, 0x00);
}
// send a packet
void rtl8139::send(void *buffer, int len)
{
	cout<<"Sending Packet...\n";
	outportl(iobase + TSAD0 + send_index * 4, (unsigned int)buffer);
	outportl(iobase + TSD0 + send_index * 4, (( TX_FIFO_THRESH & 0x7e0 ) << 11 )|(unsigned int)len);
	send_index = (send_index + 1) % 4;
}

//receive a packet
unsigned short rtl8139::receive(void *buffer)
{
	cout<<"Receive Packet...\n";
	if((inportb(iobase + RTL8139_CMD) & BUFFER_IS_EMPTY) != 0)
		return 0;
	while((inportb(iobase + RTL8139_CMD) & BUFFER_IS_EMPTY) == 0)
	{
		packet *pkg = (packet*)((unsigned int)rec_buffer + received_index);
		received_index = (received_index + pkg->size + 4 + 3) & ~3;
		memcpy(buffer,pkg,pkg->size);
		outportw(iobase + CAPR, received_index - 16);
		outportw(iobase + ISR, 1); // clear rhe isr reg so that we can get new packet
		inportw(iobase + ISR);
	}
	return 1;
}
// read and populate MAC ID
void rtl8139::read_mac()
{
	for(int i=0;i<6;i++)
	{
		mac_id[i] = inportb(iobase+i);
	}
}

void rtl8139::get_mac(unsigned char *buffer)
{
	memcpy(buffer,mac_id,6);
	*(buffer + 7) = '\0';
}

void rtl8139::show_mac()
{
	cout<<"MAC ID = ";
	for(int i=0;i<6;i++)
	{
		cout.flags(hex);
		cout<<(unsigned int)mac_id[i]<<":";
		cout.flags(dec);
	}
	cout<<"\n";
}
void rtl8139::info()
{
	cout.flags(hex|showbase);
	cout<<"IO base @"<<(unsigned short)iobase;
	cout.flags(dec);
	cout<<" using IRQ "<<(unsigned short)irq<<" ";	
	if (inportb(iobase + MSR) & (1 << 2))
		cout<<"Link fail ";
	else
		cout<<"Link ok ";
	if (inportb(iobase + MSR) & (1 << 3))
		cout<< "10 ";
	else
		cout<<"100 ";
	if(inportb(iobase + BMCR) & (1 << 8))
		cout<<"full ";
	else
		cout<<"half ";
	if(inportb(iobase + BMSR) & (1 << 2))
		cout<<"up ";
	else
		cout<<"down ";
	
	show_mac();
}

void analyze_pkt()
{
	unsigned char anbuff[1500];
	rtl_dev->receive(anbuff);
	hex_dump(anbuff,60);
	cout<<"==========================\n";
	//delete[] anbuff;
	
}
void rtl8139::irq_handler(void *sd)
{
	// so we got an IRQ Eh... our device is working..
	// now check if the IRQ is for receive or is it for transmit.
	unsigned short volatile isr_val;
	
	isr_val = inportw( iobase + ISR);
	if(isr_val & TXOK)
	{
		// we transmitted a packet
		
		
		cout<<"TX ok\n";
	}
	else if(isr_val & RXOK)
	{
		cout<<"RX ok\n"; 
		analyze_pkt();
	}
	else 
	{
		cout<<"Other ISR ="<<isr_val;
	}
	outportw( iobase + ISR, isr_val); //make TXok bit low again so we will get next TX interrupt
	inportw( iobase + ISR);	
	
}


void detect_netdev()
{
	pci_bus *pb = pci_bus::Instance();
	pci_dev *net_dev = NULL;
	unsigned char teststr[7];
	cout<<"Trying to detect PCI net device\n";
	cout<<"Checking PCI bus ...\n";
	net_dev = pb->get_dev((unsigned char)0x02, (unsigned char)0x00); //PCI network = 0x02 ETHERNET = 0x00
	if(net_dev)
	{
		cout<<"Found Ethernet device on Bus :"<<net_dev->bus<<" device :"<<net_dev->dev<<" func :" \
			<<net_dev->func<<" irq "<<(unsigned short)net_dev->irq<<" ";	
		cout<<vendor_to_string(net_dev->common->vendor_id);
		cout<<vendor_device_to_string(net_dev->common->vendor_id,net_dev->common->device_id)<<"\n";
		cout<<(unsigned int)pci_read_irq(net_dev->bus,net_dev->dev,net_dev->func)<<"\n";
		if((net_dev->common->vendor_id == 0x10ec) && (net_dev->common->device_id==0x8139))
		{
			rtl_dev = new rtl8139;
			rtl_dev->init(get_bar(net_dev,0)&(~0x3), net_dev->irq);
			rtl_dev->show_mac();
			rtl_dev->info();
			//rtl_dev->send((char *)rtl_dev, 60);
		}
		else
			cout<<"Unsupported device...\n";
	}
	
}


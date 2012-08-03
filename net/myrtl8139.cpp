#include "OStream.h"
#include "low-io.h"
#include "pci.h"
#include "myrtl8139.h"
#include "irq.h"
#include "timer.h"
#include "utils.h"
using namespace String;
void rtl8139::init(unsigned short base, unsigned char intr)
{
	cout<<"Initializing RTL 8139 NIC...";
	iobase = base;
	irq = intr;
	outportb(iobase + CONFIG1, 0); //poweron
	outportb(iobase + RTL8139_CMD, RESET); // software reset the chip
	while(inportb(iobase + RTL8139_CMD) & RESET); // loop until the reset actually happens
	read_mac();
	rec_buffer = new unsigned char[8192+16]; // get 8kb+16 byte receive buffer
	outportl(iobase + RBSTART, (unsigned int)rec_buffer); // set the rx buffer start address
	outportw(iobase + IMR, RXOK | TXOK); // generate ieq for rxok and txok
	outportw(iobase + ISR, 0x0005);     // enable IRQ
	outportl(iobase + RCR, (AAP | APM | AM | AB | AR | WRAP) & ~(unsigned int)1); // receiver config register
	outportb(iobase + RTL8139_CMD, RECEIVER_ENABLE | TRANSMITTER_ENABLE); // enable the tx and rx mechanism
	IRQ::install_handler(irq, irq_handler);
	IRQ::enable_irq(irq);
	cout<<"Done\n";
}
// send a packet
void rtl8139::send(void *buffer, int len)
{
	cout<<"Sending Packet...\n";
	outportl(iobase + TSAD0 + send_index * 4, (unsigned int)buffer);
	outportl(iobase + TSD0 + send_index * 4,(unsigned int)len);
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
		cout<<"up\n";
	else
		cout<<"down\n";
}
void rtl8139::irq_handler(void *sd)
{
	cout<<"RTL 8139 handler\n";
}
rtl8139 *rtl_dev;
//==============================Driver def ends ============================

// ARP protocol packet structure
typedef struct proto_arp {
	unsigned short hard_type;
	unsigned short prot_type;
	unsigned char hard_size;
	unsigned char prot_size;
	unsigned short op;	
	unsigned char sender_mac[6];
	unsigned int sender_ipv4;
	unsigned char target_mac[6];
	unsigned int target_ipv4;
} __attribute((packed)) proto_arp_t;
enum {
	ARP_OP_REQUEST = 0x100,
	ARP_OP_REPLY = 0x200,
	ARP_OP_RARP_REQUEST,
	ARP_OP_RARP_REPLY
};

enum {
	ARP_HARD_TYPE_ETHERNET = 0x100
};
typedef struct packet_t
{
	unsigned char dest_mac[6];
	unsigned char src_mac[6];
	unsigned short proto;
}__attribute((packed)) packet;
void test_req_arp()
{
	proto_arp_t *discover = new proto_arp_t;
	String::memset(discover,0,sizeof(proto_arp_t));
	discover->hard_type = ARP_HARD_TYPE_ETHERNET;
	discover->prot_type = 8;
	discover->hard_size = 6;
	discover->prot_size = 4;
	discover->op = ARP_OP_REQUEST;
	packet *pack = new packet;
	String::memset(pack->dest_mac,0xff,6);
	rtl_dev->get_mac(pack->src_mac);
	//if(rtl_dev)
	//	String::memcpy(pack->src_mac,rtl_dev->mac_id,6);
	pack->proto=0x0806;
	
	unsigned char *arp_pack = new unsigned char[14+sizeof(proto_arp_t)];
	String::memcpy(arp_pack,pack,14);
	String::memcpy(arp_pack+14,(unsigned char *)discover,sizeof(proto_arp_t));
	cout<<"sending=>\n";
	hex_dump(arp_pack,60);
	//hex_dump(arp_pack,14+sizeof(proto_arp_t));
	//if(rtl_dev)
	//	rtl_8139_send(rtl_dev,arp_pack,14+sizeof(proto_arp_t));
	rtl_dev->send(arp_pack, 14 + sizeof(proto_arp_t));
	//my_timer->sleep(100);
	unsigned char arp_rec[1024];
	unsigned char timeout=10;
	/*while((!rtl_8139_receive(rtl_dev,(unsigned char *)arp_rec)>0) && timeout)
	{
		timeout--;
		my_timer->sleep(10);
	}*/
		
	if(rtl_dev->receive((unsigned char *)arp_rec)!=0)
		hex_dump(arp_rec,256);

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
		if((net_dev->common->vendor_id == 0x10ec) && (net_dev->common->device_id==0x8139))
		{
			rtl_dev = new rtl8139;
			rtl_dev->init(get_bar(net_dev,0)&(~0x3), net_dev->irq);
			rtl_dev->show_mac();
			rtl_dev->info();
		}
	}
}

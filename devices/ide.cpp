// ide driver
#include "ide.h"
#include "pci.h"
#include "low-io.h"
#include "timer.h"
#define BM_IDE_CMDREG_PRI	0x00
#define BM_IDE_STATREG_PRI	0x02
#define BM_IDE_PRDTAB_ADDR_PRI	0x04
#define BM_IDE_CMDREG_SEC	0x08
#define BM_IDE_STATREG_SEC	0x0A
#define BM_IDE_PRDTAB_ADDR_SEC	0x0C

typedef struct ide_dev
{
	unsigned short port;
	unsigned char inerrupt;
}ide_dev;
// this function takes the command reg base and control reg base
// returns true if the address specified is a ata controler
bool ide_dev_present(unsigned short port_cmd,unsigned short port_ctrl)
{
	unsigned char temp1,temp2;
	TIMER *tmr=TIMER::Instance();
	outportb(port_cmd+SECT_CNT_REG,0x55);
	outportb(port_cmd+LBA_LOW_REG,0xAA);
	outportb(port_cmd+SECT_CNT_REG,0xAA);
	outportb(port_cmd+LBA_LOW_REG,0x55);
	outportb(port_cmd+SECT_CNT_REG,0x55);
	outportb(port_cmd+LBA_LOW_REG,0xAA);
	tmr->sleep(10);
	temp1=inportb(port_cmd+SECT_CNT_REG);
	temp2=inportb(port_cmd+LBA_LOW_REG);
	if((temp1==0x55)&&(temp2==0xAA))
	{
		outportb(port_ctrl,0x08|0x04|0x02);	//soft reset ????
		tmr->sleep(10);
		outportb(port_ctrl, 0x08|0x02);
		tmr->sleep(10);
		return true;
	}
	else
		return false;
}

// this will check if there is an PCI IDE controller present or not
// we here assumed there is only one or none
// There can be more than one but....
pci_dev *ide_detect()
{
	pci_dev *dev;
	pci_bus *pb=pci_bus::Instance();
	dev=pb->get_dev((unsigned char) 0x01,(unsigned char)0x01);
	if(dev!=NULL)
	{
		cout<<vendor_to_string(dev->common->vendor_id) \
                <<vendor_device_to_string(dev->common->vendor_id,dev->common->device_id) \
		<<" Present on bus="<<dev->bus<<":"<<dev->dev<<":"<<dev->func \
		<<"\n";
		return dev;
	}
	return NULL;
}
bool is_bus_master(pci_dev *t)
{
	// to know if the device is bus master or not check the 7th bit 
	// in programming interface register
	if(t->common->Prog_if & (0x01<<7))
		return true;
	else
		return false;
}
bool pci_ide_setup(pci_dev *t)
{
	unsigned int cmdbase,ctrlbase,bmidebase,idetim;
	//unsigned int found=0;	
	cout<<"PIIX IO space ";
	if((t->common->cmd_reg & 0x01) ==0)
		cout<<"disabled\n";
	else
		cout<<"enabled\n";
	if(is_bus_master(t))
		cout<<"bus master\n";
	else
		cout<<"Not bus master\n";
	// h-landis ata driver suggested this
	for(int ctrlnum=0;ctrlnum<=1;ctrlnum++)
	{
		cmdbase=get_bar(t,0+(8*ctrlnum)); //ctrlnum =0 pri , ctrlnum=1 sec
		ctrlbase=get_bar(t,1+(8*ctrlnum));
		bmidebase=get_bar(t,4);// common for pri and sec
		if(cmdbase==0xffff || ctrlbase==0xffff || bmidebase==0xffff)
			continue;
		cmdbase &= 0xfffe;
		ctrlbase &= 0xfffe;
		bmidebase &= 0xfffe;
		if(cmdbase == 0) // if our card doesn't give us ports
		{
			// use defaults
			if(ctrlnum==0) 
			{
				cmdbase=0x1f0;
				ctrlbase=0x3f0;
			}
			else
			{
				cmdbase=0x170;
				ctrlbase=0x370;
			}
		}
		else
		{
			ctrlbase -=4;
		}
		if(ctrlnum)
			bmidebase +=8;
		cout.flags(hex|showbase);
		cout<<"cmd="<<cmdbase<<" ctrl="<<ctrlbase<<" bmide="<<bmidebase<<"\n";
		if (ide_dev_present((unsigned short)cmdbase,(unsigned short)ctrlbase))
			cout<<cmdbase<<" device present\n";
		else
			cout<<cmdbase<<" device not present\n";
		cout.flags(dec);
	} //for loop ends
	// within above loop we detected controller
	// we have to put those information in a list or array
	// then from another function we will scan them and detect devices
	// connected to them
	idetim= *(unsigned int*)t+0x40;
	cout<<"idetim "<<idetim<<"\n";
	/* Hope fully here some how we have to setup the 
           bar to do IO dma or memory
           where is the logic any one????
        */
	return true; // just return true for now
}
void display_ide()
{
	pci_dev *dev;
	dev=ide_detect();
	if(dev!=NULL)
	{
		cout.flags(hex|showbase);
		cout<<"cmd reg="<<dev->common->cmd_reg<<"\n";
		cout<<"bar0="<<get_bar(dev,0)<<":"<<get_bar_type(dev,0)<<"\n";
		cout<<"bar1="<<get_bar(dev,1)<<":"<<get_bar_type(dev,1)<<"\n";
		cout<<"bar2="<<get_bar(dev,2)<<":"<<get_bar_type(dev,2)<<"\n";
		cout<<"bar3="<<get_bar(dev,3)<<":"<<get_bar_type(dev,3)<<"\n";
		cout<<"bar4="<<get_bar(dev,4)<<":"<<get_bar_type(dev,4)<<"\n";
		cout<<"bar5="<<get_bar(dev,5)<<":"<<get_bar_type(dev,5)<<"\n";
		cout.flags(dec);
	}
	pci_ide_setup(dev);
}


// ide driver
#include "pci.h"
typedef struct ide_dev
{
	unsigned short port;
	unsigned char inerrupt;
}ide_dev;
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
bool pci_ide_setup(pci_dev *t)
{
	unsigned int pci_ide_bar,idetim;	
	cout<<"PIIX IO space ";
	if((t->common->cmd_reg & 0x01) ==0)
		cout<<"disabled\n";
	else
		cout<<"enabled\n";
	pci_write_config_word(t->bus,t->dev,t->func,0x04,t->common->cmd_reg|0x04);
	pci_ide_bar=get_bar(t,4);
	idetim= *(unsigned int*)t+0x40;
	cout<<"idetim "<<idetim<<"\n";
	/* Hope fully here some how we have to setup the 
           bar to do IO dma or memory
           where is the logic any one????
        */
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


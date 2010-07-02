//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008-2010 //
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// PCI Bus specific defs                                //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef __PCI_H__
#define __PCI_H__
#include "singleton.h"
#define PCI_BASE_REG 0xCFC
#define PCI_DATA_REG 0xCF8

char *class_to_string(unsigned char classcode,unsigned char subclass);
// #include "pcihdr.h" // for vendor device and function def
typedef struct PCI_common
{
	unsigned short vendor_id;
	unsigned short device_id;
	unsigned short cmd_reg;
	unsigned short status_reg;
	unsigned char revision_id;
	unsigned char Prog_if;
	unsigned char subclass;
	unsigned char classcode;
	unsigned char cacheline_size;
	unsigned char latency;
	unsigned char header_type;
	unsigned char BIST;		//self test
}__attribute((packed)) PCI_common;
struct non_bridge
{
	unsigned long base_address0 ;
	unsigned long base_address1 ;
	unsigned long base_address2 ;
	unsigned long base_address3 ;
	unsigned long base_address4 ;
	unsigned long base_address5 ;
	unsigned long CardBus_CIS ;
	unsigned short  subsystem_vendorID ;
	unsigned short  subsystem_deviceID ;
	unsigned long expansion_ROM ;
	unsigned char  cap_ptr ;
	unsigned char  reserved1[3] ;
	unsigned long reserved2[1] ;
	unsigned char  interrupt_line ;
	unsigned char  interrupt_pin ;
	unsigned char  min_grant ;
	unsigned char  max_latency ;
	unsigned long device_specific[48] ;
}__attribute((packed));
struct bridge
{
	unsigned long base_address0 ;
	unsigned long base_address1 ;
	unsigned char  primary_bus ;
	unsigned char  secondary_bus ;
	unsigned char  subordinate_bus ;
	unsigned char  secondary_latency ;
	unsigned char  IO_base_low ;
	unsigned char  IO_limit_low ;
	unsigned short  secondary_status ;
	unsigned short  memory_base_low ;
	unsigned short  memory_limit_low ;
	unsigned short  prefetch_base_low ;
	unsigned short  prefetch_limit_low ;
	unsigned long prefetch_base_high ;
	unsigned long prefetch_limit_high ;
	unsigned short  IO_base_high ;
	unsigned short  IO_limit_high ;
	unsigned long reserved2[1] ;
	unsigned long expansion_ROM ;
	unsigned char  interrupt_line ;
	unsigned char  interrupt_pin ;
	unsigned short  bridge_control ;
	unsigned long device_specific[48] ;
}__attribute((packed));
struct cardbus
{
	unsigned long ExCa_base ;
	unsigned char  cap_ptr ;
	unsigned char  reserved05 ;
	unsigned short  secondary_status ;
	unsigned char  PCI_bus ;
	unsigned char  CardBus_bus ;
	unsigned char  subordinate_bus ;
	unsigned char  latency_timer ;
	unsigned long memory_base0 ;
	unsigned long memory_limit0 ;
	unsigned long memory_base1 ;
	unsigned long memory_limit1 ;
	unsigned short  IObase_0low ;
	unsigned short  IObase_0high ;
	unsigned short  IOlimit_0low ;
	unsigned short  IOlimit_0high ;
	unsigned short  IObase_1low ;
	unsigned short  IObase_1high ;
	unsigned short  IOlimit_1low ;
	unsigned short  IOlimit_1high ;
	unsigned char  interrupt_line ;
	unsigned char  interrupt_pin ;
	unsigned short  bridge_control ;
	unsigned short  subsystem_vendorID ;
	unsigned short  subsystem_deviceID ;
	unsigned long legacy_baseaddr ;
	unsigned long cardbus_reserved[14] ;
	unsigned long vendor_specific[32] ;
}__attribute((packed));
typedef struct confadd
{
    unsigned char reg:8;
    unsigned char func:3;
    unsigned char dev:5;
    unsigned char bus:8;
    unsigned char rsvd:7;
    unsigned char enable:1;
} confadd_t;
struct pci_dev
{
	unsigned int bus,dev,func;
	PCI_common *common;
	unsigned int devi[60];
	pci_dev *next;
	pci_dev *prev;
};
typedef enum bar_type{
	BAR_TYPE_MEM=0,
	BAR_TYPE_IO
}bar_type_t;
class pci_bus: public Singleton<pci_bus>
{
	private:
		pci_dev *pci_list;
		pci_dev *end;
		unsigned int num_dev;
		//pci_bus(): pci_list(NULL),end(NULL),num_dev(0){};
	public:		
		void list_dev();
		void scan();
		pci_dev *get_dev(unsigned short vendor,unsigned short device);
		pci_dev *get_dev(unsigned char classcode,unsigned char subclass);
};
const char *vendor_to_string(unsigned short vend_id);
const char *vendor_device_to_string(unsigned short vendor,unsigned short device);
unsigned int read_pci(int bus, int dev, int func, int reg, int uint8_ts);
void write_pci(int bus, int dev, int func, int reg, unsigned int v, int uint8_ts);
unsigned char pci_read_config_byte(int bus, int dev, int func, int reg);
unsigned short pci_read_config_word(int bus, int dev, int func, int reg);
unsigned int pci_read_config_dword(int bus, int dev, int func, int reg);
void pci_write_config_byte(int bus, int dev, int func, int reg, unsigned char val);
void pci_write_config_word(int bus, int dev, int func, int reg, unsigned short val);
void pci_write_config_dword(int bus, int dev, int func, int reg, unsigned int val);
void pci_bus_scan();
unsigned int get_bar(pci_dev *dev,int bar_num);
bar_type_t get_bar_type(pci_dev *dev, int bar_num);
#endif


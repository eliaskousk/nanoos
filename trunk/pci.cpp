/*PCI Test code*/
#include "low-io.h"
#include "string.h"
#include "OStream.h"
struct pci_sig
	{
		unsigned char sig[4];
		void *entry;
		unsigned char rev_lev;
		unsigned char length;
		unsigned char cksum;
		unsigned char res[5];
	}__attribute__ ((packed));
void probe_pci()
{
	char *rom_beg=(char *)0xe0000;
	char *rom_end=(char *)0xFFFFF;
	struct pci_sig *pci=(struct pci_sig *)rom_beg;
	cout<<"trying PCI probe by rom signature"<<"\n";	
	do{
		
		if(String::strncmp((const char*)pci->sig,"_32_",4)==0)
		{
			int i;			
			for (i=0;i<4;i++)
			cout<<(pci->sig[i]);			
			cout<<"PCI BIOS found at \t"<<(unsigned int)pci;
			cout<<"entry point at "<<(unsigned int)pci->entry<<"\n";
		}
		pci+=16;
	}while((unsigned int)pci<0xfffff);
}


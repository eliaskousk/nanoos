/*ramdisk
 a ram disk is a memory area which is created by OS(kernel/app/boot loader)
 a ram disk has the following property
	1) start address
	2) size
	3) file allocation table
	4) data area start
In our case we will over simplify the design
1) the start address and total size is obtained from GRUB
2) file allocation table will be an array of file_info so it is a fixed thing
3) the file_info will contain the following
	1) file name [32]
	2) file size unsigned int byte
	3) offset in ramdisk data ( the cumulative sum of sizes before the 
           file) unsigned int
*/
struct rd_file_info
{
	char sig[7]="NanoOS"; // signature we will check in module
	char name[32];
	unsigned int size;
	unsigned int offset;
}
struct ramdisk_info
{
	unsigned int start_addr;
	unsigned int size;
	rd_file_info rd_fallo_tbl[512]; // 512 files no sub dirs
	unsigned int data_start;  // start of data area
}

/* the ramdisk_info structure will be at the begining of the ramdisk area
   we will have to write a utility program which will create a ramdisk
   by reading files from a directory or a config file */
class ramdisk
{
	private:
		ramdisk_info *rdinfo;
	public:
		ramdisk() : rdinfo(NULL){};
		void set_rdinfo(unsigned int rd_module_load_addr)
		{
			if(rd_module_load_addr>0)
			rdinfo=(struct ramdisk_info*)rd_module_load_addr;
		}
		bool rd_open(){ if(rdinfo!=NULL) return true;return false;}
		void rd_disp_info()
		{
			if(rdinfo==NULL)
				return;
			else
			{
				cout<<"RAM Disk Info\n";
				cout<<"Load address :"<<rdinfo->start_addr<<"\n";
				cout<<"Size         :"<<rdinfo->size<<"\n";
				cout<<"Data begin   :"<<rdinfo->data_start<<"\n";
			}
		}			
	// this should return true if the ramdisk is loaded
	// this information is obtained from GRUB
	// we will scan for modules in kernel main()
	// if we found a module loaded by GRUB then check it
	// if it is our Ramdisk then return true
	

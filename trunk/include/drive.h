//drive.h
#include "fdc.h"
//#include "ide.h"
enum{
	FD=0,
	HD=1,
	CD=2,
};
enum{
	FAT=0,
	EXT2=1,
	UNK=2,
};
typedef struct drvgeom
{
	int heads;
	int cyl; //tracks
	int spt; //sectors per track
	unsigned int total_sectors;
	int block_size;
	int gap3;
	int fmt_gap3; //gap3<<2
	unsigned char fmt_fill;
	unsigned char fmt_shift;
}drvgeom;
class drive
{
	public:	
	char name[10]; //fd0,hd0,hd01
	drv geom;
	char buff[512];
	int type; //FD,HD,CD
	int fstype; //FAT EXT2 ..
	bool (*read_sector)(int sect,unsigned char *buf,long unsigned int numsect); // callback function for device type
	bool (*write_sector)(int sect,unsigned char *buf,long unsigned int numsect);//  -do-
	//void (*reset)(){};
	void detect_fs(){};
	drive(char *nam)
	{
		if(String::strncmp(nam,"fd",2)==0)		
		{
			String::strcpy(name,nam);
			log_disk(&geom);
			read_sector= read_block;
			write_sector= write_block;
		}
		else
		{
			cout<<"[Unknown type] --- are to be implemented\n";
		}
		if(geom.heads>2)
		{
			cout<<"Geometry	of "<<name<<" heads = "<<geom.heads<<" cylinders= "<<geom.tracks<<" spt= "<<geom.spt<<"\n";
		}			
	}
	~drive(){};
};



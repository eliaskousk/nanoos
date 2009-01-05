#include "low-io.h"
#include "OStream.h"
#include "fdc.h"
#include "floppy.h"
//fdc.cpp

int fd_num[2]; //we will put our cmos result for floppy here. currently we are tracking only 2 floppies.
char fd_type[][50]={"no floppy drive", "360kb 5.25in floppy drive", "1.2mb 5.25in floppy drive", "720kb 3.5in", "1.44mb 3.5in", "2.88mb 3.5in"};


//detect the floppy by reading CMOS
int detect_floppy_cmos()
{
	unsigned char result;
	int num_drv=0,i;
	outportb(0x70,0x10); //Set the index to 10h
	result=inportb(0x71); //read the byte at index 10h
	//get the first nibble i.e first drive.
	fd_num[0]=result >> 4;
	fd_num[1]=result & 0xf;
	for(i=0;i<2;i++)
	{
		if(fd_num[i]>0)
		{
			num_drv++;
			cout<<fd_type[fd_num[i]]<<"\n";
			if(fd_num[i]==4)
			flpdrvs[i]=new floppy();
		}	
	}
	cout<<"Total floppy drives = "<<num_drv<<"\n";
	return num_drv;
}



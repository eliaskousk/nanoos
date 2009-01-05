//floppy.cpp
#include "floppy.h"
#include "dma.h"
#include "OStream.h"
void floppy::floppy_handler(IDT::regs *r)
{
	_interrupt_occurred |= (unsigned short)FLP_IRQ;
	//outportb(0x20, 0x20);
}
void floppy::floppy_send_byte(unsigned short ioadr, unsigned char byte)
{
	unsigned char msr;
	unsigned timeout;

	for(timeout = 128; timeout != 0; timeout--)
	{
		msr = inportb(ioadr + FDC_MSR);
		if((msr & 0xC0) == 0x80)
		{
			outportb(ioadr + FDC_DATA, byte);
			return;
		}
	}
/* xxx - else? */
}
int floppy::floppy_get_byte(unsigned short ioadr)
{
	unsigned char msr;
	unsigned timeout;

	for(timeout = 128; timeout != 0; timeout--)
	{
		msr = inportb(ioadr + FDC_MSR);
		if((msr & 0xD0) == 0xD0)
			return inportb(ioadr + FDC_DATA);
	}
	return -1; /* timeout */
}
void floppy::floppy_motor_on(floppy_t *dev)
{
	unsigned short ioadr, t;
	unsigned char unit;

	cout<<"floppy_motor_on:\n";
	ioadr = dev->blkdev.io.adr[0];
	unit = dev->blkdev.unit;
	t = dev->timeout;
/* cancel motor-off timeout.
-1 tells timer interrupt not to decrement  */
	dev->timeout = -1;
	if(t != 0)
		return; /* motor already on */
/* xxx - top four bits enable motors, b3 enables DMA and I/O,
b2 enables the the FDC chip, b1 and b0 are drive select

xxx - need a static global variable to cache the value written to FDC_DOR

xxx - floppy_select_drive() should be a separate function, maybe */
	outportb(ioadr + FDC_DOR, 0x1C);//unit?);
/* delay 500ms for motor to spin up
xxx - use interrupts here? */
	msleep(500);
}
void floppy::floppy_motor_off(floppy_t *dev)
{
	cout<<"floppy_motor_off:\n";
	dev->timeout = 36; /* 18 ticks, ~2 sec */
}
int floppy::floppy_await_interrupt(floppy_t *fdev)
{
	unsigned short irq, ioadr;
	unsigned char status[7];
	int temp;

	ioadr = fdev->blkdev.io.adr[0];
	irq = fdev->blkdev.io.irq;
	cout<<"floppy_await_interrupt:\n";
/* wait up to 1 second for IRQ 6 */
	temp = await_interrupt(irq, 1000);
	if(temp == 0) /* timeout */
	{
		cout<<"floppy_await_interrupt: timeout\n";
		return -1;
	}
/* read in command result bytes */
	for(temp = 0; temp < 7; temp++)
	{
		if((inportb(ioadr + FDC_MSR) & 0x10) == 0)
			break;
		status[temp] = floppy_get_byte(ioadr);
	}
/* perform "sense interrupt" */
	floppy_send_byte(ioadr, 0x08); /* CMD_SENSEI */
	fdev->sr0 = floppy_get_byte(ioadr);
	fdev->curr_track = floppy_get_byte(ioadr);

/* */

	cout<<"status byte returned by floppy controller: "<<status[0]<<"\n";
	cout<<"status returned by floppy controller:\n";
	cout<<"drive "<<(status[0] & 3)<<", head "<<((status[0] >> 2) & 1)<<"\n";
	cout<<"not ready on R/W or SS access to head 1: "<<((status[0] >> 3) & 1)<<"\n";
	cout<<"equipment check: "<<((status[0] >> 4) & 1)<<"\n";
	cout<<"seek complete: "<<((status[0] >> 5) & 1)<<"\n";
	cout<<"last command status: ";
	switch(status[0] >> 6)
	{
		case 0:	cout<<"success\n";	break;
		case 1: cout<<"started OK but abnormal termination\n"; break;
		case 2: cout<<"invalid command issued\n"; break;
		case 3: cout<<"abnormal termination because READY changed state\n"; break;
	}

	return status[0];
}
int floppy::floppy_seek(floppy_t *dev, unsigned char track)
{
	unsigned short ioadr;

	cout<<"floppy_seek: seeking track "<<track<<"\n";
/* already there? */
	if(dev->curr_track == track)
		return 0;
/* turn on the motor */
	floppy_motor_on(dev);
/* send the actual command bytes */
	ioadr = dev->blkdev.io.adr[0];
	floppy_send_byte(ioadr, 0x0F);	/* CMD_SEEK */
	floppy_send_byte(ioadr, 0);
	floppy_send_byte(ioadr, track);
/* wait until seek finished */
	if(floppy_await_interrupt(dev) < 0)
		return -1;
/* now let head settle for 15ms
xxx - use interrupt */
	msleep(15);
	cout<<"floppy_seek: sr0="<<dev->sr0<<" (should be 0x20), curr_track="<<dev->curr_track<<"(should be "<<track<<")\n";
	floppy_motor_off(dev);
/* check that seek worked */
	if(dev->sr0 != 0x20 || dev->curr_track != track)
		return -1;
	return 0;
}
int floppy::floppy_recalibrate(floppy_t *fdev)
{
	unsigned short ioadr;

	cout<<"floppy_recalibrate:\n";
	ioadr = fdev->blkdev.io.adr[0];
	floppy_send_byte(ioadr, 0x07);
	floppy_send_byte(ioadr, 0);
	return floppy_await_interrupt(fdev) < 0;
}
int floppy::floppy_error(request_t *req)
{
	floppy_t *fdev;
	unsigned short ioadr;

	cout<<"*** floppy_error: ***\n";
/* increment req->errors */
	(req->errors)++;
	if(req->errors >= 3)
	{
		cout<<"3 errors\n";
		return -1;
	}
/* */
	fdev = (floppy_t *)req->dev;
	ioadr = fdev->blkdev.io.adr[0];
/* if disk change, seek track 1 (xxx - one? not zero?) */
	if(inportb(ioadr + FDC_DIR) & 0x80)
	{
		cout<<"floppy_error: disk change\n";
		if(floppy_seek(fdev, 1) != 0)
		{
/* if that fails, we're screwed... */
			cout<<"could not seek to track 1\n";
			floppy_motor_off(fdev);
			return -1;
		}
	}
	cout<<"Trying to recalibrateing....\n"; 	
	return floppy_recalibrate(fdev);
}
int floppy::floppy_rw(request_t *req)
{
	unsigned short ioadr, sector, head, num_bytes, num_blks;
	unsigned long blk, adr;
	blkdev_t *blkdev;
	floppy_t *fdev;
	int temp;

	cout<<"floppy_rw:\n";
	//req->errors=0;
	fdev = (floppy_t *)req->dev;
/* if floppy_t struct had a magic value in it, we could validate it here */
	blkdev = &fdev->blkdev;
/* select drive and spin up the disk */
	floppy_motor_on(fdev);
	ioadr = blkdev->io.adr[0];
	do
	{
		blk = req->blk;
		sector = blk % blkdev->sectors + 1;
		blk /= blkdev->sectors;
		head = blk % blkdev->heads;
		blk /= blkdev->heads;
		cout<<"floppy_rw: CHS="<<(unsigned int)blk<<":"<<head<<":"<<sector<<"\n";
/* do not cross a track boundary */
		num_blks = min(req->num_blks, blkdev->sectors -
			req->blk % blkdev->sectors);
		num_bytes = num_blks * blkdev->bytes_per_blk;
		cout<<"floppy_rw: transferring "<<num_blks<<" blocks ("<<num_bytes<<" bytes)\n";
/* disk change? */
		if(inportb(ioadr + FDC_DIR) & 0x80)
		{
			cout<<"diskchange....????\n";
			goto ERR;
		}
/* seek to correct track */
		if(floppy_seek(fdev, blk) != 0)
		{
			cout<<"Traseek failed???\n";
			goto ERR;
		}
/* program data rate (500K/s) */
		outportb(ioadr + FDC_CCR, 0);
/* copy data to 64K-aligned conventional memory buffer */
		if(req->cmd == BLK_CMD_WRITE)
			String::strncpy((char*)req->buf,(char*)LMEM ,num_bytes);
		adr = LMEM_OFF;

		if(req->cmd == BLK_CMD_READ)
		{
			cout<<"floppy_rw: read\n";
			floppy_send_byte(ioadr, 0xE6);//CMD_READ);
/* xxx - 2 is DMA number. Can't use blkdev->io.dma; it is a mask
Dammit, I forgot to subtract one here to convert length into terminal count.
Made that error with my sound software, too. */
			if(dma_read(2, adr, num_bytes - 1) != 0)
			{
				cout<<"floppy_rw: error from dma_read()\n";
				goto ERR;
			}
		}
		else// if(req->cmd == BLK_CMD_WRITE)
		{
			cout<<"floppy_rw: write\n";
			floppy_send_byte(ioadr, 0xC5);//CMD_WRITE);
			if(dma_write(2, adr, num_bytes - 1) != 0)
			{
				cout<<"floppy_rw: error from dma_write()\n";
				goto ERR;
			}
		}
/* head and drive select */
		floppy_send_byte(ioadr, (head << 2) | blkdev->unit);
/* C, H, S, N */
		floppy_send_byte(ioadr, blk);
		floppy_send_byte(ioadr, head);
		floppy_send_byte(ioadr, sector);
		floppy_send_byte(ioadr, 2);	/* 512 bytes/sector */
/* EOT */
		floppy_send_byte(ioadr, blkdev->sectors);
/* gap 3 length for 1.44M or 1.68M floppies */
		if(blkdev->sectors == 18)
			floppy_send_byte(ioadr, 0x1B);
		else //if(blkdev->sectors == 21)
			floppy_send_byte(ioadr, 0x1C);
		floppy_send_byte(ioadr, 0xFF);	/* DTL = unused */
/* wait up to 1 second for IRQ 6 */
		temp = floppy_await_interrupt(fdev);
		if((temp < 0) || (temp & 0xC0))
		{
ERR:
			temp = floppy_error(req);
/* if floppy_error() returns non-zero, then we are doomed... */
			if(temp != 0)
				return temp;
/* ...else try again */
			else
				continue;
		}
/* copy data from 64K-aligned conventional memory buffer */
		if(req->cmd == BLK_CMD_READ)
			String::strncpy((char *)LMEM, (char *)req->buf, num_bytes);
/* advance pointers */
		req->num_blks -= num_blks;
		req->blk += num_blks;
		req->buf += num_bytes;
	} while (req->num_blks != 0);
	floppy_motor_off(fdev);
	return 0;
}
void floppy::floppy_read_block(unsigned int block, unsigned char *userbuffer)
{
	request.cmd=BLK_CMD_READ;
	request.buf=userbuffer;
	request.blk=block;
	request.num_blks=1;
	floppy_rw(&request);
}



floppy::floppy()
{
	fdev.blkdev.io.dma = 0x04;
	fdev.blkdev.io.irq = FLP_IRQ;
	fdev.blkdev.io.adr[0] = 0x3F0;
/* 8 bytes at 0x3F0
xxx - steer clear of 0x3F6? Win95 says 0x3F2-0x3F5, but we need 0x3F7 too */
	fdev.blkdev.io.span[0] = 8;

//	blkdev.unit = ?;
	fdev.blkdev.num_blks = 2880;
	fdev.blkdev.bytes_per_blk = 512;
	fdev.blkdev.sectors = 18; /* xxx - probe this, somehow */
	fdev.blkdev.heads = 2;
	fdev.blkdev.cyls = 80;
	fdev.curr_track = -1;
	request.dev=&fdev;
	request.errors=0;
	cout<<"installing floppy interrupt\n";
	IRQ::install_handler(6,floppy_handler);
}
void dump(void *data_p, unsigned count)
{
	unsigned char *data = (unsigned char *)data_p;
	unsigned i, j;

	while(count != 0)
	{
		for(i = 0; i < BPERL; i++)
		{
			if(count == 0)
				break;
			cout<<data[i]<<" ";
			count--;
		}
		cout<<"\t";
		for(j = 0; j < i; j++)
		{
			if(data[j] < ' ')
				cout<<".";
			else
				cout<<data[j];
		}
		cout<<"\n";
		data += BPERL;
	}
}
floppy *flpdrvs[2];

//floppy.h
#ifndef __FLOPPY_H__
#define __FLOPPY_H__
#include "blockdev.h"
#include "irq.h"

/* offsets from 0x3F0 */
#define FDC_DOR		2	/* Digital Output Register */
#define FDC_MSR		4	/* Main Status Register */
#define FDC_DATA	5	/* Data Register */

#define	FDC_DIR		7	/* Digital Input Register (input) */
#define FDC_CCR		7	/* Configuration Control Register (output) */
#define FLP_IRQ		1<<6

typedef struct
{
/* generic block device info */
	blkdev_t blkdev;
/* information specific to floppy drive */
	unsigned short timeout;
	unsigned char curr_track, sr0;
} floppy_t;
class floppy : public blockdev
{
	public:
		floppy();
		~floppy(){};	
		int floppy_rw(request_t *req);
		static void floppy_handler(IDT::regs *r);
		void floppy_read_block(unsigned int block,unsigned char *userbuffer);
	private:
		void floppy_send_byte(unsigned short ioadr, unsigned char byte);
		int floppy_get_byte(unsigned short ioadr);
		void floppy_motor_on(floppy_t *dev);
		void floppy_motor_off(floppy_t *dev);
		int floppy_await_interrupt(floppy_t *fdev);
		int floppy_seek(floppy_t *dev, unsigned char track);
		int floppy_recalibrate(floppy_t *fdev);
		int floppy_error(request_t *req);
		
		floppy_t fdev;
};
extern floppy *flpdrvs[2];

#endif

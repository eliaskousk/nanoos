/*
GazOS Operating System
Copyright (C) 1999  Gareth Owen <gaz@athene.co.uk>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef INCLUDE_DMA
#define INCLUDE_DMA
#define	NUM_DMAS	8
#define	DMA_READ	0x44
#define	DMA_WRITE	0x48

/* 8-bit DMAC registers */
#define	DMA_BASE	0
#define	DMA_MASK	0x0A	/* mask register (write only) */
#define	DMA_MODE	0x0B	/* mode register */
#define	DMA_FF		0x0C	/* MSB/LSB flip flop */
#define	DMA_RESET	0x0D	/* master clear */

/* Upper 4 bits (8 bits ?) of 20-bit (24-bit?) address
come from the DMAC page registers: */
static const unsigned char _page_reg_ioadr[NUM_DMAS] =
{
	0x87, 0x83, 0x81, 0x82, 0, 0x8B, 0x89, 0x8A
};

void dma_xfer(unsigned char channel, unsigned long address, unsigned int length, unsigned char read);
int dma_read(unsigned char chan, unsigned long linear,unsigned long tc);
int dma_write(unsigned char chan, unsigned long linear,unsigned long tc);

#endif


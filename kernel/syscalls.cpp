//syscall cpp
#include "OStream.h"
#include "irq.h"
void syscall(void *sp)
{
	IDT::regs *r=(IDT::regs *)sp;
	cout<<"Syscall:";
	switch(r->eax)
	{
		
		case 1:	// do syscall 1
			
		case 2:	
		case 3:	
		case 4:	
		case 5:	
		case 6:	
		case 7:	
		case 8:	
		case 9:	
		case 10:	// do syscall
			cout<<r->eax;
			break; 
			
		default:
			cout<<" Unknown / unimplemented number "<<r->eax;
	}
	cout<<"\n";	
}
void init_syscall()
{
	IRQ::install_handler(21,syscall);
	IRQ::enable_irq(21);
}

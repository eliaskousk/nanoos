//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2009//
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// CPU: CPU Identification routine                      //
//                                                      //
//////////////////////////////////////////////////////////
/*CPU IDENTIFICATION PROGRAM modified by Ashok Shankar Das
**     1- Moved assembly codes to separate asm file "gcpun.asm"
**     2- This file now is a pure C file can be compiled
**        with any 32-bit c-compiler tested with gcc(djgpp-3.2.3)
**CPU ID routines for 386+ CPU's
**Written by Phil Frisbie, Jr. (pfrisbie@geocities.com)
**Parts adapted from the cpuid algorithm by Robert Collins(rcollins@x86.org)
**and from Cyrix sample code.
**See cpu.txt for more details on Intel and Cyrix codes.
*/

#include "string.h"
#include "gcpu.h"
#include "OStream.h"
#include "kheap.h"
#ifndef TRUE
	#define TRUE	1
	#define FALSE	0
#endif



void cyrix_type(cpu_t *mcpu)
{
	char temp, orgc2, newc2, orgc3, newc3;
	int cr2_rw=FALSE, cr3_rw=FALSE, type;

	type = UNKNOWN;

	/* Test Cyrix c2 register read/writable */

	orgc2 = _cyrix_read_reg(0xc2);	/* get current c2 value */

	newc2 = orgc2 ^ CR2_MASK;	/* toggle test bit */
	_cyrix_write_reg(0xc2, newc2);		/* write test value to c2 */
	_cyrix_read_reg(0xc0);			/* dummy read to change bus */

	if (_cyrix_read_reg(0xc2) != orgc2)	/* did test bit toggle */
		cr2_rw = TRUE;		/* yes bit changed */

	_cyrix_write_reg(0xc2, orgc2);		/* return c2 to original value */

	/* end c2 read writeable test */

	/* Test Cyrix c3 register read/writable */

	orgc3 = _cyrix_read_reg(0xc3);		/* get current c3 value */

	newc3 = orgc3 ^ CR3_MASK;	/* toggle test bit */
	_cyrix_write_reg(0xc3, newc3);		/* write test value to c3 */
	_cyrix_read_reg(0xc0);			/* dummy read to change bus */

	if (_cyrix_read_reg (0xc3) != orgc3)	/* did test bit change */
		cr3_rw = TRUE;		/* yes it did */

	_cyrix_write_reg (0xc3, orgc3);		/* return c3 to original value */

	/* end c3 read writeable test */

	if ((cr2_rw && cr3_rw) || (!cr2_rw && cr3_rw)) /*DEV ID register ok */
	{
		/* <<<<<<< READ DEVICE ID Reg >>>>>>>> */
		type = _cyrix_read_reg (0xfe);	/* lower byte gets IDIR0 */
	}

	else if (cr2_rw && !cr3_rw)	/* Cx486S A step */
	{
		type = Cx486S_a;	/* lower byte */
	}

	else if (!cr2_rw && !cr3_rw)	/* Pre ID Regs. Cx486SLC or DLC */
	{
		type = Cx486_pr;	/* lower byte */
	}

	/* This could be broken down more, but is it needed? */
	if (type < 0x30 || type > 0xfc)
	{
		mcpu->cpu_family = 4;		/* 486 class-including 5x86 */
		mcpu->cpu_model = 15;		/* Unknown */
	}
	else if (type < 0x50)
	{
		mcpu->cpu_family = 5;		/* Pentium class-6x86 and Media GX */
		mcpu->cpu_model = 15;		/* Unknown */
	}
	else
	{
		mcpu->cpu_family = 6;		/* Pentium || class- 6x86MX */
		mcpu->cpu_model = 15;		/* Unknown */
		mcpu->cpu_mmx = TRUE;
	}
}




/* the following code is taken from Jens olsson's Cpu-id code
** this code displays some more info some flags about intel and amd cpus
*/
void capability(cpu_inf t,cpu_t *mcpu)
{
	unsigned long C,Q;
	
	if(String::strcmp(mcpu->cpu_vendor, "GenuineIntel")==0)
	{
			
		String::strcpy(mcpu->Comp1[0],"FPU"); //Floating Point Unit
		String::strcpy(mcpu->Comp1[1],"VME"); //Virtual Mode Extension
		String::strcpy(mcpu->Comp1[2],"DE"); //Debugging Extension
		String::strcpy(mcpu->Comp1[3],"PSE"); //Page Size Extension
		String::strcpy(mcpu->Comp1[4],"TSC"); //Time Stamp Counter
		String::strcpy(mcpu->Comp1[5],"MSR"); //Model Specific Registers
		String::strcpy(mcpu->Comp1[6],"PAE"); //Physical Address Extesnion
		String::strcpy(mcpu->Comp1[7],"MCE"); //Machine Check Extension
		String::strcpy(mcpu->Comp1[8],"CX8"); //CMPXCHG8 Instruction
		String::strcpy(mcpu->Comp1[9],"APIC"); //On-chip APIC Hardware
		String::strcpy(mcpu->Comp1[10],""); //Reserved
		String::strcpy(mcpu->Comp1[11],"SEP"); //SYSENTER SYSEXIT
		String::strcpy(mcpu->Comp1[12],"MTRR"); //Machine Type Range Registers
		String::strcpy(mcpu->Comp1[13],"PGE"); //Global Paging Extension
		String::strcpy(mcpu->Comp1[14],"MCA"); //Machine Check Architecture
		String::strcpy(mcpu->Comp1[15],"CMOV"); //Conditional Move Instrction
		String::strcpy(mcpu->Comp1[16],"PAT"); //Page Attribute Table
		String::strcpy(mcpu->Comp1[17],"PSE-36"); //36-bit Page Size Extension
		String::strcpy(mcpu->Comp1[18],"PSN"); //96-bit Processor Serial Number
		String::strcpy(mcpu->Comp1[19],"CLFSH"); //CLFLUSH Instruction
		String::strcpy(mcpu->Comp1[20],""); //Reserved
		String::strcpy(mcpu->Comp1[21],"DS"); //Debug Trace Store
		String::strcpy(mcpu->Comp1[22],"ACPI"); //ACPI Support
		String::strcpy(mcpu->Comp1[23],"MMX"); //MMX Technology
		String::strcpy(mcpu->Comp1[24],"FXSR"); //FXSAVE FXRSTOR (Fast save and restore)
		String::strcpy(mcpu->Comp1[25],"SSE"); //Streaming SIMD Extensions
		String::strcpy(mcpu->Comp1[26],"SSE2"); //Streaming SIMD Extensions 2
		String::strcpy(mcpu->Comp1[27],"SS"); //Self-Snoop
		String::strcpy(mcpu->Comp1[28],"HTT"); //Hyper-Threading Technology
		String::strcpy(mcpu->Comp1[29],"TM"); //Thermal Monitor Supported
		String::strcpy(mcpu->Comp1[30],"IA-64"); //IA-64 capable
		String::strcpy(mcpu->Comp1[31],""); //Reserved
	}
	else if(String::strcmp(mcpu->cpu_vendor, "AuthenticAMD")==0)
	{
			
		String::strcpy(mcpu->Comp1[0],"FPU"); //Floating Point Unit
		String::strcpy(mcpu->Comp1[1],"VME"); //Virtual Mode Extension
		String::strcpy(mcpu->Comp1[2],"DE"); //Debugging Extension
		String::strcpy(mcpu->Comp1[3],"PSE"); //Page Size Extension
		String::strcpy(mcpu->Comp1[4],"TSC"); //Time Stamp Counter
			
		String::strcpy(mcpu->Comp1[5],"MSR"); //Model Specific Registers
		String::strcpy(mcpu->Comp1[6],"PAE"); //Physical Address Extesnion
		String::strcpy(mcpu->Comp1[7],"MCE"); //Machine Check Extension
		String::strcpy(mcpu->Comp1[8],"CX8"); //CMPXCHG8 Instruction
		String::strcpy(mcpu->Comp1[9],"APIC"); //On-chip APIC Hardware
		
		String::strcpy(mcpu->Comp1[10],""); //Reserved
		String::strcpy(mcpu->Comp1[11],"SEP"); //SYSENTER SYSEXIT
		String::strcpy(mcpu->Comp1[12],"MTRR"); //Machine Type Range Registers
		String::strcpy(mcpu->Comp1[13],"PGE"); //Global Paging Extension
		String::strcpy(mcpu->Comp1[14],"MCA"); //Machine Check Architecture
		String::strcpy(mcpu->Comp1[15],"CMOV"); //Conditional Move Instrction
		String::strcpy(mcpu->Comp1[16],"PAT"); //Page Attribute Table
		String::strcpy(mcpu->Comp1[17],"PSE-36"); //36-bit Page Size Extension
		String::strcpy(mcpu->Comp1[18],""); //?
		String::strcpy(mcpu->Comp1[19],"MPC"); //MultiProcessing Capable (I made this short up, correct?)
		String::strcpy(mcpu->Comp1[20],""); //Reserved
		String::strcpy(mcpu->Comp1[21],""); //?
		String::strcpy(mcpu->Comp1[22],"MIE"); //AMD Multimedia Instruction Extensions (I made this short up, correct?)
		String::strcpy(mcpu->Comp1[23],"MMX"); //MMX Technology
		String::strcpy(mcpu->Comp1[24],"FXSR"); //FXSAVE FXRSTOR (Fast save and restore)
		String::strcpy(mcpu->Comp1[25],"SSE"); //Streaming SIMD Extensions
		
		String::strcpy(mcpu->Comp1[26],""); //?
		String::strcpy(mcpu->Comp1[27],""); //?
		String::strcpy(mcpu->Comp1[28],""); //?
		String::strcpy(mcpu->Comp1[29],""); //?
		String::strcpy(mcpu->Comp1[30],"3DNowExt"); //3DNow Instruction Extensions (I made this short up, correct?)
		String::strcpy(mcpu->Comp1[31],"3DNow"); //3DNow Instructions (I made this short up, correct?)
		
	}
	
	for(C=1, Q=0;Q<32;C*=2, Q++)
	{
		mcpu->dComp1Supported[Q]=(t.reg_edx&C)!=0?1:0;
	}

	mcpu->dBrand=t.reg_ebx&0xFF;
	mcpu->dCacheLineSize=((String::strcmp(mcpu->Comp1[19], "CLFSH")==0)&&(mcpu->dComp1Supported[19]==1))?((t.reg_ebx>>8)&0xFF)*8:-1;
	mcpu->dLogicalProcessorCount=((String::strcmp(mcpu->Comp1[28], "HTT")==0)&&(mcpu->dComp1Supported[28]==1))?((t.reg_ebx>>16)&0xFF):-1;
	//This only works on P4 or later, must check for that in the future
	mcpu->dLocalAPICID=((t.reg_ebx>>24)&0xFF);
	
}

void check_cpu(cpu_t *mcpu) /* This is the function to call to set the globals */
{
	char 	*unknown_vendor = "NoVendorName";
	char	*cyrix = "CyrixInstead";
	long vendor_temp[3];
	cpu_inf t;
	String::memset(mcpu->cpu_vendor, 0, 16);
	if (_is_cpuid_supported())
	{
		mcpu->cpu_cpuid = TRUE;
		t.reg_eax = t.reg_ebx = t.reg_ecx = t.reg_edx = 0;
		_get_cpuid_info(0,&t);
		mcpu->cpuid_levels = t.reg_eax;
		vendor_temp[0] = t.reg_ebx;
		vendor_temp[1] = t.reg_edx;
		vendor_temp[2] = t.reg_ecx;
		String::memcpy(mcpu->cpu_vendor, vendor_temp, 12);
		if (mcpu->cpuid_levels > 0)
		{
			t.reg_eax = t.reg_ebx = t.reg_ecx = t.reg_edx = 0;
			_get_cpuid_info (1,&t);
			mcpu->cpu_family = ((t.reg_eax>>8) & 0xf);
			mcpu->cpu_model = ((t.reg_eax>>4) & 0xf);
			mcpu->cpu_stepping=(t.reg_eax & 0xf);
			mcpu->cpu_ext_family=((t.reg_eax>>20) & 0xff);
			mcpu->cpu_ext_model=((t.reg_eax>>16) & 0xf);
			switch(((t.reg_eax>>12)&0x7))
			{
				case 0:
					String::strcpy(mcpu->dType, "Original");
					break;
				case 1:
					String::strcpy(mcpu->dType, "OverDrive");
					break;
				case 2:
					String::strcpy(mcpu->dType, "Dual");
					break;
			}
			mcpu->cpu_fpu = (t.reg_edx & 1 ? TRUE : FALSE);
			mcpu->cpu_mmx = (t.reg_edx & 0x800000 ? TRUE: FALSE);
			
			capability(t,mcpu);
			
		}
	}
	else
	{
		String::memcpy(mcpu->cpu_vendor, unknown_vendor, 12);
		mcpu->cpu_fpu = _is_fpu();
		if (!_is_486())
		{
			if (_is_386DX())	/* It is a 386DX */
			{
				mcpu->cpu_family = 3;	/* 386 */
				mcpu->cpu_model = 0;	/* DX */
			}
			else		/* It is a 386SX */
			{
				mcpu->cpu_family = 3;	/* 386 */
				mcpu->cpu_model = 1;	/* SX */
			}
		}
		else			/* It is a 486+ */
		{
			if(_is_cyrix())
			{
				String::memcpy(mcpu->cpu_vendor, cyrix, 12);
				cyrix_type(mcpu);
			}
			else
			{
				mcpu->cpu_family = 4;	/* 486 */
				mcpu->cpu_model = 15;	/* unknown */
			}
		}
	}
}

int cpuinfo(void) /* Sample program */
{
	cpu_t *my_cpu = (cpu_t *)kmalloc(sizeof(cpu_t));
	
	check_cpu(my_cpu);
	cout<<"CPU has cpuid instruction? ";	
	if(my_cpu->cpu_cpuid)
		cout<<"yes";
	else 
		cout<<"no";
	cout<<"\n";
	cout<<"CPU vender is "<< my_cpu->cpu_vendor<<"\t";
	cout<< my_cpu->dType<<"\n";
	cout<<"CPU family is "<< my_cpu->cpu_family<<"\n";
	cout<<"CPU extended family is "<< my_cpu->cpu_ext_family<<"\n";
	cout<<"CPU model is "<< my_cpu->cpu_model<<"\n";
	cout<<"CPU extended model is "<< my_cpu->cpu_ext_model<<"\n";
	cout<<"CPU stepping is "<< my_cpu->cpu_stepping<<"\n";
	cout<<"CPU has fpu? ";
	if(my_cpu->cpu_fpu)	
		cout<<"yes";
	else
		cout<<"no";
	cout<<"\n";
	cout<<"CPU has mmx? ";
	if(my_cpu-> cpu_mmx)
		cout<< "yes";
	else
		cout<< "no";
	cout<<"\n";
	cout<<"Cache line size "<<(unsigned int)my_cpu->dCacheLineSize<<"\n";
	cout<<"Logical Processor Count "<<(unsigned int)my_cpu->dLogicalProcessorCount<<"\n";
	cout<<"Local ACPI ID "<<(unsigned int)my_cpu->dLocalAPICID<<"\n";
	cout<<"Supported Flags : ";
	for(int i=0;i<32;i++)
	{
		if(my_cpu->dComp1Supported[i])
			cout<<my_cpu->Comp1[i]<<" ";
	}
	cout<<"\n";
	delete my_cpu;
	return 1;
}

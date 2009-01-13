//gcpu.h
#ifndef __GCPU_H__
#define __GCPU_H__

#define UNKNOWN   0xff
#define Cx486_pr  0xfd  /* ID Register not supported, software created */
#define Cx486S_a  0xfe  /* ID Register not supported, software created */
#define CR2_MASK  0x4   /* LockNW */
#define CR3_MASK  0x80  /* Resereved bit 7 */

typedef struct ret_regs
{
	unsigned int reg_eax;
	unsigned int reg_ebx;
	unsigned int reg_ecx;
	unsigned int reg_edx;
} __attribute__ ((packed)) cpu_inf;


typedef struct cpu
{
	char	cpu_vendor[16];		/* Vendor String, or Unknown */
	char    dType[20];
	int	cpu_family;		/* 3=386, 4=486, 5=Pentium, 6=PPro, 7=Pentium ||?, etc */
	int	cpu_model;		/* other details such as SX, DX, overdrive, etc. */
	int     cpu_stepping;           /* stepping*/
	int     cpu_ext_family;		/* extended family*/
	int	cpu_ext_model;		/*extended model info*/
	int	cpu_fpu;	/* TRUE or FALSE */
	int	cpu_mmx;	/* TRUE or FALSE */
	int	cpu_cpuid;	/* Whether the cpu supported the cpuid instruction */
				/* if TRUE, you can trust the information returned */
				/* if FALSE, be careful... ;) */
	int	cpuid_levels;	
	char Comp1[10][32];
	int dComp1Supported[32];
	int 	dBrand,
		dCacheLineSize,
		dLogicalProcessorCount,
		dLocalAPICID;

}__attribute__ ((packed)) cpu_t;

extern "C" int _is_486(void); /* return TRUE for 486+, and FALSE for 386 */
extern "C" int _is_386DX(void); /* return TRUE for 386DX, and FALSE for 386SX */
extern "C" int _is_fpu(void); /*true for yes FPU */
extern "C" int _is_cyrix(void); /*true for Yes*/
extern "C" int _cyrix_read_reg(int);
extern "C" void _cyrix_write_reg(int,int);
extern "C" int _is_cpuid_supported(void); /* true if Supported*/
extern "C" void _get_cpuid_info(int cpuid_levels,cpu_inf *t); /* This is so simple! */

void cyrix_type(cpu_t *mcpu);
void capability(cpu_inf t,cpu_t *mcpu);
void check_cpu(cpu_t *mcpu); /* This is the function to call to set the globals */
int cpuinfo(void);
#endif //__GCPU_H__

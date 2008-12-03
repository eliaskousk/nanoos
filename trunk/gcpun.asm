;************************************************************
;*********         GCPUN.ASM               NASM FILE  *******
;*********COPYRIGHT(C)2003-2004(GPL) ASHOK SHANKAR DAS*******
;*********            ashok_s_das@yahoo.com           *******
;************************************************************
[BITS 32]
global _is_486
global _is_386DX
global _is_fpu
global _is_cyrix
global _is_cpuid_supported
global _get_cpuid_info
global _cyrix_read_reg
global _cyrix_write_reg

segment .data
segment .bss
;_reg_eax resd 1h
;_reg_ebx resd 1h
;_reg_ecx resd 1h
;_reg_edx resd 1h

segment .text
;
;_is_486 if a 486 is present
;
_is_486:
	pushf ;			/* save EFLAGS */
	pop	eax ;		/* get EFLAGS */
	mov	ecx,eax;		/* temp storage EFLAGS */
	xor	eax,0x40000;"	/* change AC bit in EFLAGS */
	push	eax ;		/* put new EFLAGS value on stack */
	popf ;			/* replace current EFLAGS value */
	pushf ;			/* get EFLAGS */
	pop	eax ;		/* save new EFLAGS in EAX */
	cmp	eax,ecx ;		/* compare temp and new EFLAGS */
	jz	a ;
	mov	eax,1 ;		/* 80486 present */
	jmp	b ;

a:
	mov	eax,0 ;"		/* 80486 not present */

b:
	push 	ecx ;		/* get original EFLAGS */
	popf ;			/* restore EFLAGS */

	ret;
;
;_is_386DX if a 386DX is present
;
_is_386DX:
	mov	edx,cr0 ;"		/* Get CR0 */
	push	edx ;"		/* save CR0 */
	and	dl,0xef;"		/* clear bit4 */
	mov	cr0,edx ;" 	/* set CR0 */
	mov	edx,cr0;"		/* and read CR0 */
	and	dl,0x10;"		/* bit4 forced high? */
	pop	edx ;"		/* restore reg w/ CR0 */
	mov	cr0,edx ;"		/* restore CR0 */
	mov	eax,1;"		/* TRUE, 386DX */
	jz	c ;
	mov	eax,0;"		/* FALSE, 386SX */

c:
	ret;
;
;_is_fpu checks if floating point unit(math co processor)is present
;
_is_fpu:
	mov	sp,bp;
	fninit ;
	mov    ax,0x5a5a;"
	push ax;
	fnstsw  [bp-2] ;"
	pop	ax
	cmp    ax,0;"
	jne     d ;" 		no fpu
	mov    eax,1;"
	jmp     e ;"

d:
	mov    eax,0;"

e:
	ret

;
;cyrix_write_reg(char index, char val) writes to cyrix cpu
;
_cyrix_write_reg:

	push	ebp;
	mov	ebp,esp;
	push	eax;
	push	ebx;
	mov	eax,dword[ebp+8]
	mov	ebx,dword[ebp+12]
	pushf;			/* save flags */
	cli;			/* clear interrupt in flags */
	out 0x22, eax;		/* tell CPU which config. register */
	mov eax,ebx;
	out 0x23, eax;		/* write to CPU config. register */
	popf;			/* restore flags */
	pop ebx;
	pop eax;
	pop ebp;
	ret
;
;_cyrix_read_reg(char index) reads from register
;
_cyrix_read_reg:
	push ebp;
	mov ebp,esp;
	mov eax,dword[ebp+8];
	pushf; 			/* save flags */
	cli;			/* clear interrupt in flags */
	out 0x22,eax;		/* tell CPU which config. register */
	in eax,0x23;		/* read CPU config, register */
	popf;			/* restore flags */
	pop ebp;
	ret

;
;is_cyrix checks if it is a cyrix cpu
;
_is_cyrix:
	xor     ax,ax ;"		/* clear eax */
	sahf ;			/* clear flags, bit 1 is always 1 in flags */
	mov     ax,5;"
	mov     bx,2;"
	div      bl ;"		/* do an operation that does not change flags */
	lahf ;			/* get flags */
	cmp     ah,2;"		/* check for change in flags */
	jne      f;"			/* flags changed not Cyrix */
	mov     eax,1;"		/* TRUE Cyrix CPU */
	jmp      g;"

f:
	mov     eax,0;"		/* FALSE NON-Cyrix CPU */
g:
	ret
;
;_is_cpuid_supported returns 1 if we can execute cpuid
;
_is_cpuid_supported:
	pushf ;			/* get extended flags */
	pop     eax ;
	mov     ebx,eax ;"	/* save current flags */
	xor     eax,0x200000;"	/* toggle bit 21 */
	push    eax ;"		/* put new flags on stack */
	popf ;"			/* flags updated now in flags */
	pushf ;"			/* get extended flags */
	pop     eax ;"
	xor     eax,ebx ;"	/* if bit 21 r/w then supports cpuid */
	jz      e0;"
	mov     eax,1;"
	jmp      e1;"

e0:

	mov     eax,0;"

e1:
	ret
;get_cpu_info(int cpuid_level, &ret_struct)
;the retstruct will contain information of the registers. it is defined in
; C source file
_get_cpuid_info:
	push ebp	; save ebp
	mov ebp,esp	; stack pointer to ebp
	push esi        ;save esi
	mov eax,dword[ebp+8] ; get first param in eax FIRST PARAM is at esp+8
	mov esi,dword[ebp+0ch] ; get the address of structure in esi which is
			       ; at esp+12d
	cpuid ;                ; execute cpuid instruction
	mov	dword[esi+0h],eax ;store eax at esi which is retstruct.reg_eax
	mov	dword[esi+04h],ebx ;store ebx at esi+4 retstruct.reg_ebx 
	mov	dword[esi+08h],ecx ;store ecx at retstruct.reg_ecx
	mov	dword[esi+0ch],edx ;store edx at retstruct.reg_edx
	pop esi ;restore esi
	pop ebp ;restore ebp
	ret

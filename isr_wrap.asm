;//////////////////////////////////////////////////////////
;// This file is a part of Nanos Copyright (C) 2008, 2009//
;// ashok.s.das@gmail.com                                //
;//////////////////////////////////////////////////////////
;// isr wrapper                                          //
;//                                                      //
;//////////////////////////////////////////////////////////
;Stolen from Bran's tutorial
; we will add our Interrupt
; Service Routines (ISRs) right here!
segment .text
global _isr0
global _isr1
global _isr2
global _isr3
global _isr4
global _isr5
global _isr6
global _isr7
global _isr8
global _isr9
global _isr10
global _isr11
global _isr12
global _isr13
global _isr14
global _isr15
global _isr16
global _isr17
global _isr18
global _isr19
global _isr20
global _isr21
global _isr22
global _isr23
global _isr24
global _isr25
global _isr26
global _isr27
global _isr28
global _isr29
global _isr30
global _isr31
global _irq0  ;IRQ 0
global _irq1
global _irq2
global _irq3
global _irq4
global _irq5
global _irq6
global _irq7
global _irq8
global _irq9
global _irq10
global _irq11
global _irq12
global _irq13
global _irq14
global _irq15   ;IRQ 15
global read_eip		; I could not find a better place to put 
				; this so put it here
extern _irq_handler ; C code 


;  0: Divide By Zero Exception
_isr0:
    cli
    push byte 0    ; A normal ISR stub that pops a dummy error code to keep 
		   ;a uniform stack frame
    push byte 0
    jmp isr_common_stub

;  1: Debug Exception
_isr1:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

;2 : NMI
_isr2:
    cli
    push byte 0
    push byte 2
    jmp isr_common_stub

;3 Breakpoint
_isr3:
    cli
    push byte 0
    push byte 3
    jmp isr_common_stub

;4 Into detected overflow
_isr4:
    cli
    push byte 0
    push byte 4
    jmp isr_common_stub

;5 Out Of Bound
_isr5:
    cli
    push byte 0
    push byte 5
    jmp isr_common_stub

;6 Invalid Opcode
_isr6:
    cli
    push byte 0
    push byte 6
    jmp isr_common_stub

;7 No Co_processor
_isr7:
    cli
    push byte 0
    push byte 7
    jmp isr_common_stub    

;  8: Double Fault Exception (With Error Code!)
_isr8:
    cli
    push byte 8 ; Note that we DON'T push a value on the stack in this one!
                ; It pushes one already!Use this type of stub for exceptions
                ; that pop error codes!
    jmp isr_common_stub

;9: co-processor segment overrun
_isr9:
    cli
    push byte 0
    push byte 9
    jmp isr_common_stub

;10 Bad TSS
_isr10:
    cli
    push byte 10; Note that we DON'T push a value on the stack in this one!
                ; It pushes one already!Use this type of stub for exceptions
                ; that pop error codes!
    jmp isr_common_stub

;11 Segment Not Present
_isr11:
    cli
    push byte 11; Note that we DON'T push a value on the stack in this one!
                ; It pushes one already!Use this type of stub for exceptions
                ; that pop error codes!
    jmp isr_common_stub

;12 Stack fault
_isr12:
    cli
    push byte 12; Note that we DON'T push a value on the stack in this one!
                ; It pushes one already!Use this type of stub for exceptions
                ; that pop error codes!
    jmp isr_common_stub

;13 GPF
_isr13:
    cli
    push byte 13; Note that we DON'T push a value on the stack in this one!
                ; It pushes one already!Use this type of stub for exceptions
                ; that pop error codes!
    jmp isr_common_stub

;14 Page Fault
_isr14:
    cli
    push byte 14; Note that we DON'T push a value on the stack in this one!
                ; It pushes one already!Use this type of stub for exceptions
                ; that pop error codes!
    jmp isr_common_stub

;15 Unknown Exception
_isr15:
    cli
    push byte 0
    push byte 15
    jmp isr_common_stub

;16 Co-Processor Fault
_isr16:
    cli
    push byte 0
    push byte 16
    jmp isr_common_stub

;17 Allignment Check Exception
_isr17:
    cli
    push byte 0
    push byte 17
    jmp isr_common_stub

;18 Machine Check Exception
_isr18:
    cli
    push byte 0
    push byte 18
    jmp isr_common_stub

;19 reserved
_isr19:
    cli
    push byte 0
    push byte 19
    jmp isr_common_stub

;20 reserved
_isr20:
    cli
    push byte 0
    push byte 20
    jmp isr_common_stub

;21 reserved
_isr21:
    cli
    push byte 0
    push byte 21
    jmp isr_common_stub

;22 reserved
_isr22:
    cli
    push byte 0
    push byte 22
    jmp isr_common_stub

;23 reserved
_isr23:
    cli
    push byte 0
    push byte 23
    jmp isr_common_stub

;24 reserved
_isr24:
    cli
    push byte 0
    push byte 24
    jmp isr_common_stub

;25 reserved
_isr25:
    cli
    push byte 0
    push byte 25
    jmp isr_common_stub

;26 reserved
_isr26:
    cli
    push byte 0
    push byte 26
    jmp isr_common_stub

;27 reserved
_isr27:
    cli
    push byte 0
    push byte 27
    jmp isr_common_stub

;28 reserved
_isr28:
    cli
    push byte 0
    push byte 28
    jmp isr_common_stub

;29 reserved
_isr29:
    cli
    push byte 0
    push byte 29
    jmp isr_common_stub

;30 reserved
_isr30:
    cli
    push byte 0
    push byte 30
    jmp isr_common_stub

;31 reserved
_isr31:
    cli
    push byte 0
    push byte 31
    jmp isr_common_stub
;============================IRQs====================
; 32: IRQ0
extern task_switch
extern end_irq
_irq0:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 32
   ; jmp irq_common_stub
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp  ; save esp in eax
    push eax      ; push it on the stack so that we will get the whole stack
                  ; inside the irq handler 
    mov eax, _irq_handler ;; call the irq_handler C function 
    call eax              ; the call will not change the eip
    pop eax               ; here recall the esp as it was before the call
                          ; irq_handler  
	push eax
	call task_switch
	push eax
	call end_irq
	pop esp
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
	sti
    iret	
; 33: IRQ1
_irq1:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 33
    jmp irq_common_stub
; 34: IRQ2
_irq2:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 34
    jmp irq_common_stub
; 35: IRQ3
_irq3:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 35
    jmp irq_common_stub
; 36: IRQ4
_irq4:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 36
    jmp irq_common_stub
; 37: IRQ5
_irq5:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 37
    jmp irq_common_stub
; 38: IRQ6
_irq6:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 38
    jmp irq_common_stub
; 39: IRQ7
_irq7:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 39
    jmp irq_common_stub
; 40: IRQ8
_irq8:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 40
    jmp irq_common_stub
; 41: IRQ9
_irq9:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 41
    jmp irq_common_stub
; 42: IRQ10
_irq10:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 42
    jmp irq_common_stub
; 43: IRQ11
_irq11:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 43
    jmp irq_common_stub
; 44: IRQ12
_irq12:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 44
    jmp irq_common_stub
; 45: IRQ13
_irq13:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 45
    jmp irq_common_stub
; 46: IRQ14
_irq14:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 46
    jmp irq_common_stub
; 47: IRQ15
_irq15:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 47
    jmp irq_common_stub

 

; This is a stub that we have created for IRQ based ISRs. This calls
; '_irq_handler' in our C code. We need to create this in an 'irq.c'
irq_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp  ; save esp in eax
    push eax      ; push it on the stack so that we will get the whole stack
                  ; inside the irq handler 
    mov eax, _irq_handler ;; call the irq_handler C function 
    call eax              ; the call will not change the eip
    pop eax               ; here recall the esp as it was before the call
                          ; irq_handler  
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret

; We call a C function in here. We need to let the assembler know
; that '_fault_handler' exists in another file
extern _fault_handler

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
isr_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10   ; Load the Kernel Data Segment descriptor!
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp   ; Push us the stack
    push eax
    mov eax, _fault_handler
    call eax       ; A special call, preserves the 'eip' register
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

read_eip:
	pop eax
	jmp eax

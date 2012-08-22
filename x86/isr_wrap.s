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
global _irq16
global _irq17
global _irq18
global _irq19
global _irq20
global _irq21
global _irq65

global read_eip		; I could not find a better place to put 
				; this so put it here
global my_test
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
	mov esp,eax
	mov eax,esp
	push eax
	call task_switch
	mov esp,eax
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
_irq16:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 48
    jmp irq_common_stub
_irq17:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 49
    jmp irq_common_stub
_irq18:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 50
    jmp irq_common_stub
_irq19:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 51
    jmp irq_common_stub 
_irq20:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 52
	jmp irq_common_stub

_irq21:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 53
	jmp irq_common_stub

_irq22:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 54
	jmp irq_common_stub

_irq23:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 55
	jmp irq_common_stub

_irq24:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 56
	jmp irq_common_stub

_irq25:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 57
	jmp irq_common_stub

_irq26:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 58
	jmp irq_common_stub

_irq27:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 59
	jmp irq_common_stub

_irq28:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 60
	jmp irq_common_stub

_irq29:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 61
	jmp irq_common_stub

_irq30:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 62
	jmp irq_common_stub

_irq31:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 63
	jmp irq_common_stub

_irq32:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 64
	jmp irq_common_stub

_irq33:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 65
	jmp irq_common_stub

_irq34:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 66
	jmp irq_common_stub

_irq35:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 67
	jmp irq_common_stub

_irq36:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 68
	jmp irq_common_stub

_irq37:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 69
	jmp irq_common_stub

_irq38:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 70
	jmp irq_common_stub

_irq39:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 71
	jmp irq_common_stub

_irq40:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 72
	jmp irq_common_stub

_irq41:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 73
	jmp irq_common_stub

_irq42:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 74
	jmp irq_common_stub

_irq43:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 75
	jmp irq_common_stub

_irq44:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 76
	jmp irq_common_stub

_irq45:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 77
	jmp irq_common_stub

_irq46:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 78
	jmp irq_common_stub

_irq47:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 79
	jmp irq_common_stub

_irq48:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 80
	jmp irq_common_stub

_irq49:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 81
	jmp irq_common_stub

_irq50:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 82
	jmp irq_common_stub

_irq51:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 83
	jmp irq_common_stub

_irq52:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 84
	jmp irq_common_stub

_irq53:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 85
	jmp irq_common_stub

_irq54:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 86
	jmp irq_common_stub

_irq55:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 87
	jmp irq_common_stub

_irq56:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 88
	jmp irq_common_stub

_irq57:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 89
	jmp irq_common_stub

_irq58:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 90
	jmp irq_common_stub

_irq59:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 91
	jmp irq_common_stub

_irq60:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 92
	jmp irq_common_stub

_irq61:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 93
	jmp irq_common_stub

_irq62:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 94
	jmp irq_common_stub

_irq63:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 95
	jmp irq_common_stub

_irq64:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 96
	jmp irq_common_stub

_irq65:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 97
	jmp irq_common_stub

_irq66:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 98
	jmp irq_common_stub

_irq67:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 99
	jmp irq_common_stub

_irq68:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 100
	jmp irq_common_stub

_irq69:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 101
	jmp irq_common_stub

_irq70:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 102
	jmp irq_common_stub

_irq71:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 103
	jmp irq_common_stub

_irq72:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 104
	jmp irq_common_stub

_irq73:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 105
	jmp irq_common_stub

_irq74:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 106
	jmp irq_common_stub

_irq75:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 107
	jmp irq_common_stub

_irq76:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 108
	jmp irq_common_stub

_irq77:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 109
	jmp irq_common_stub

_irq78:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 110
	jmp irq_common_stub

_irq79:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 111
	jmp irq_common_stub

_irq80:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 112
	jmp irq_common_stub

_irq81:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 113
	jmp irq_common_stub

_irq82:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 114
	jmp irq_common_stub

_irq83:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 115
	jmp irq_common_stub

_irq84:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 116
	jmp irq_common_stub

_irq85:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 117
	jmp irq_common_stub

_irq86:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 118
	jmp irq_common_stub

_irq87:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 119
	jmp irq_common_stub

_irq88:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 120
	jmp irq_common_stub

_irq89:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 121
	jmp irq_common_stub

_irq90:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 122
	jmp irq_common_stub

_irq91:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 123
	jmp irq_common_stub

_irq92:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 124
	jmp irq_common_stub

_irq93:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 125
	jmp irq_common_stub

_irq94:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 126
	jmp irq_common_stub

_irq95:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 127
	jmp irq_common_stub

_irq96:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 128
	jmp irq_common_stub

_irq97:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 129
	jmp irq_common_stub

_irq98:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 130
	jmp irq_common_stub

_irq99:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 131
	jmp irq_common_stub

_irq100:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 132
	jmp irq_common_stub

_irq101:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 133
	jmp irq_common_stub

_irq102:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 134
	jmp irq_common_stub

_irq103:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 135
	jmp irq_common_stub

_irq104:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 136
	jmp irq_common_stub

_irq105:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 137
	jmp irq_common_stub

_irq106:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 138
	jmp irq_common_stub

_irq107:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 139
	jmp irq_common_stub

_irq108:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 140
	jmp irq_common_stub

_irq109:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 141
	jmp irq_common_stub

_irq110:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 142
	jmp irq_common_stub

_irq111:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 143
	jmp irq_common_stub

_irq112:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 144
	jmp irq_common_stub

_irq113:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 145
	jmp irq_common_stub

_irq114:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 146
	jmp irq_common_stub

_irq115:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 147
	jmp irq_common_stub

_irq116:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 148
	jmp irq_common_stub

_irq117:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 149
	jmp irq_common_stub

_irq118:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 150
	jmp irq_common_stub

_irq119:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 151
	jmp irq_common_stub

_irq120:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 152
	jmp irq_common_stub

_irq121:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 153
	jmp irq_common_stub

_irq122:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 154
	jmp irq_common_stub

_irq123:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 155
	jmp irq_common_stub

_irq124:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 156
	jmp irq_common_stub

_irq125:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 157
	jmp irq_common_stub

_irq126:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 158
	jmp irq_common_stub

_irq127:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 159
	jmp irq_common_stub

_irq128:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 160
	jmp irq_common_stub

_irq129:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 161
	jmp irq_common_stub

_irq130:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 162
	jmp irq_common_stub

_irq131:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 163
	jmp irq_common_stub

_irq132:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 164
	jmp irq_common_stub

_irq133:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 165
	jmp irq_common_stub

_irq134:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 166
	jmp irq_common_stub

_irq135:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 167
	jmp irq_common_stub

_irq136:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 168
	jmp irq_common_stub

_irq137:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 169
	jmp irq_common_stub

_irq138:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 170
	jmp irq_common_stub

_irq139:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 171
	jmp irq_common_stub

_irq140:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 172
	jmp irq_common_stub

_irq141:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 173
	jmp irq_common_stub

_irq142:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 174
	jmp irq_common_stub

_irq143:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 175
	jmp irq_common_stub

_irq144:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 176
	jmp irq_common_stub

_irq145:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 177
	jmp irq_common_stub

_irq146:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 178
	jmp irq_common_stub

_irq147:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 179
	jmp irq_common_stub

_irq148:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 180
	jmp irq_common_stub

_irq149:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 181
	jmp irq_common_stub

_irq150:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 182
	jmp irq_common_stub

_irq151:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 183
	jmp irq_common_stub

_irq152:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 184
	jmp irq_common_stub

_irq153:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 185
	jmp irq_common_stub

_irq154:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 186
	jmp irq_common_stub

_irq155:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 187
	jmp irq_common_stub

_irq156:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 188
	jmp irq_common_stub

_irq157:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 189
	jmp irq_common_stub

_irq158:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 190
	jmp irq_common_stub

_irq159:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 191
	jmp irq_common_stub

_irq160:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 192
	jmp irq_common_stub

_irq161:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 193
	jmp irq_common_stub

_irq162:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 194
	jmp irq_common_stub

_irq163:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 195
	jmp irq_common_stub

_irq164:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 196
	jmp irq_common_stub

_irq165:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 197
	jmp irq_common_stub

_irq166:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 198
	jmp irq_common_stub

_irq167:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 199
	jmp irq_common_stub

_irq168:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 200
	jmp irq_common_stub

_irq169:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 201
	jmp irq_common_stub

_irq170:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 202
	jmp irq_common_stub

_irq171:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 203
	jmp irq_common_stub

_irq172:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 204
	jmp irq_common_stub

_irq173:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 205
	jmp irq_common_stub

_irq174:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 206
	jmp irq_common_stub

_irq175:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 207
	jmp irq_common_stub

_irq176:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 208
	jmp irq_common_stub

_irq177:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 209
	jmp irq_common_stub

_irq178:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 210
	jmp irq_common_stub

_irq179:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 211
	jmp irq_common_stub

_irq180:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 212
	jmp irq_common_stub

_irq181:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 213
	jmp irq_common_stub

_irq182:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 214
	jmp irq_common_stub

_irq183:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 215
	jmp irq_common_stub

_irq184:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 216
	jmp irq_common_stub

_irq185:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 217
	jmp irq_common_stub

_irq186:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 218
	jmp irq_common_stub

_irq187:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 219
	jmp irq_common_stub

_irq188:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 220
	jmp irq_common_stub

_irq189:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 221
	jmp irq_common_stub

_irq190:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 222
	jmp irq_common_stub

_irq191:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 223
	jmp irq_common_stub

_irq192:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 224
	jmp irq_common_stub

_irq193:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 225
	jmp irq_common_stub

_irq194:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 226
	jmp irq_common_stub

_irq195:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 227
	jmp irq_common_stub

_irq196:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 228
	jmp irq_common_stub

_irq197:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 229
	jmp irq_common_stub

_irq198:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 230
	jmp irq_common_stub

_irq199:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 231
	jmp irq_common_stub

_irq200:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 232
	jmp irq_common_stub

_irq201:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 233
	jmp irq_common_stub

_irq202:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 234
	jmp irq_common_stub

_irq203:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 235
	jmp irq_common_stub

_irq204:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 236
	jmp irq_common_stub

_irq205:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 237
	jmp irq_common_stub

_irq206:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 238
	jmp irq_common_stub

_irq207:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 239
	jmp irq_common_stub

_irq208:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 240
	jmp irq_common_stub

_irq209:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 241
	jmp irq_common_stub

_irq210:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 242
	jmp irq_common_stub

_irq211:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 243
	jmp irq_common_stub

_irq212:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 244
	jmp irq_common_stub

_irq213:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 245
	jmp irq_common_stub

_irq214:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 246
	jmp irq_common_stub

_irq215:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 247
	jmp irq_common_stub

_irq216:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 248
	jmp irq_common_stub

_irq217:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 249
	jmp irq_common_stub

_irq218:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 250
	jmp irq_common_stub

_irq219:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 251
	jmp irq_common_stub

_irq220:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 252
	jmp irq_common_stub

_irq221:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 253
	jmp irq_common_stub

_irq222:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 254
	jmp irq_common_stub

_irq223:
	cli
	push byte 0	; Note that these don't push an error code on the stack:
			; We need to push a dummy error code
	push byte 255
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


my_test:
	mov	eax,8
	int	53
	ret

read_eip:
	pop eax
	jmp eax


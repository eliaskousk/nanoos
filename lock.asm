;//////////////////////////////////////////////////////////
;// This file is a part of Nanos Copyright (C) 2008-2010 //
;// ashok.s.das@gmail.com                                //
;//////////////////////////////////////////////////////////
;// lock/unlock code to be used in mutex class           //
;//                                                      //
;//////////////////////////////////////////////////////////

[BITS 32]
[SECTION .text]

global init_lock
global test_and_set
;// init lock sets the value of lock_t pointer to 0
;// void init_lock(lock_t *l)

init_lock:
	mov dword eax, [esp+4]
	mov dword [eax],0
	ret
;// test_and_set puts the val in lock_t pointer and returns val
; int test_and_set(unsigned int val,lock_t *l)
test_and_set:
	mov eax,[esp+4] ; store val in eax
	mov edx,[esp+8] ; store lock_t in edx
	lock            ; lock so no other process can execute the code
	xchg [edx],eax  ; excahnge the value of eax and value at edx 
	ret 


.arm
.fpu vfp
.align(4);
.section .rodata

.global customBreak
.type customBreak, %function
customBreak:
	SVC 0x3C
	BX LR
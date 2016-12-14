.arm
.align(4);

.global sleep
.type sleep, %function	
sleep:
SVC		0xA
BX		LR


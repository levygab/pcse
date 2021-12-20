# pcse

you need qemu to use this. 

command to start : 

make debug 
gdb kernel.bin
target remote : 1234
b kernel_start
c

the order of fonction use is in the file start.c

it creates a small OS.
New version (pcse2) in this git will be better.

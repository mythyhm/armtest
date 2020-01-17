#LIBPATH  :=  /opt/arm-linux-gcc-3.4.5/lib/gcc/arm-linux/3.4.5
#arm-linux-ld -T armtest.lds -Ttext 0x33F80000  crt0.o lowlevel_init.o timer.o irq.o board.o libgeneric.a -L$(LIBPATH) -lgcc -o armtest_elf

LIBPATH := /opt/arm-linux-gcc-3.4.5/lib/gcc/arm-linux/3.4.5

armtest:
	arm-linux-gcc -c -o crt0.o crt0.S
	arm-linux-gcc -c -o lowlevel_init.o lowlevel_init.S
	arm-linux-gcc -c -o dma.o dma.c
	arm-linux-gcc -c -o irq.o irq.c
	arm-linux-gcc -c -o board.o board.c
	arm-linux-gcc -c -o timer.o timer.c
	arm-linux-ld -Bstatic -T armtest.lds -Ttext 0x33F80000  crt0.o lowlevel_init.o irq.o dma.o board.o -o armtest_elf
	arm-linux-objcopy --gap-fill=0xff -O binary -S armtest_elf armtest.bin
clean:
	rm -f *.o *elf* *.bin

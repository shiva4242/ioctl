obj-m = simple_char.o

all:
	make -C/lib/modules/$(shell uname -r)/build M=$(shell pwd) modules


clean :
	rm -f *.ko*.o *.mod.c

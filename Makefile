obj-m += kernelroll.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) 

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

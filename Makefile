obj-m += solution.o

KERN_DIRR := /lib/modules/$(shell uname -r)/build

all:
	make -C $(KERN_DIRR) M=$(PWD) modules

clean:
	make -C $(KERN_DIRR) M=$(PWD) clean

obj-m += solution.o
obj-m += checker.o

KERN_DIR := /lib/modules/$(shell uname -r)/build

all:
	make -C $(KERN_DIR) M=$(PWD) modules

clean:
	make -C $(KERN_DIR) M=$(PWD) clean


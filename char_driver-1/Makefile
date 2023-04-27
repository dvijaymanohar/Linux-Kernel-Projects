obj-m := exp_chrdev.o

KERNELDIR	?=/lib/modules/$(shell uname -r)/build

#KERNELDIR ?= /home/padmanabha/Linux/Kernel/kernel_2.6.10/linux-2.6.10_mvl401_LSP_01_30_00_082
#KERNELDIR ?= /home/padmanabha/Linux/Kernel/kernel_2.6.18/linux-2.6.18_pro500
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	$(CC) -o test_app test_app.c

.PHONY=clean
clean:
	rm -rf *.o *.??* .*.cmd .tmp_versions test_app



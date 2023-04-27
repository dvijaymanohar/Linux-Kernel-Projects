obj-m := usb_syfs.o

KERNELDIR	?=/lib/modules/$(shell uname -r)/build

#KERNELDIR ?= /home/padmanabha/Linux/Kernel/kernel_2.6.10/linux-2.6.10_mvl401_LSP_01_30_00_082
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

.PHONY=clean
clean:
	rm -rf *.o


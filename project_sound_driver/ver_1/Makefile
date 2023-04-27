# Check for KERNELRELEASE

ifneq ($(KERNELRELEASE), )
	obj-m := cs4280.o
	
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -f *.ko *.mod.* *.o *.mod.c
endif

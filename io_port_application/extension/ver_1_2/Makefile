# Check for KERNELRELEASE

ifneq ($(KERNELRELEASE), )
	obj-m := led_pattern_module.o
	
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -f *.ko *.mod.* *.o *.mod.c
endif

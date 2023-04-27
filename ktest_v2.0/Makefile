ifneq ($(KERNELRELEASE),)
	obj-m:= ktest_v2.0.o

else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD:= $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif

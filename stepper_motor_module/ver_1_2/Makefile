# This is the piece of code for Module building
ifneq ($(KERNELRELEASE), )
	obj-m := stepper_motor_module.o
	
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

# Clean up the directory
clean:
	rm -f *.ko *.mod.* *.o *.mod.c

# I've simplified the recursive prcedure for
# inserting module and making device file for it.
insert: insert_mod make_node

insert_mod:
	insmod stepper_motor_module.ko
make_node:
	mknod stepper_drv c 253 0

# This piece of code automates the procedure for
# removing the module and device file.
remove: remove_mod remove_nod

remove_mod:
	rmmod stepper_motor_module
remove_nod:
	rm -f stepper_drv
endif

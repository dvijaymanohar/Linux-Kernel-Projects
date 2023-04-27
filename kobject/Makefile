obj-$(CONFIG_SAMPLE_KOBJECT) += kobject-example.o kset-example.o

KBUILD_CFLAGS += -w

obj-m := kobject-example.o

all:
	make -w -C /lib/modules/$(shell uname -r)/build M=$(PWD) -I$(shell pwd) modules
	rm -rf *.o  *.mod.* *.cmd .module* modules* Module* .*.cmd .tmp* *~

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -rf test *.o *.ko *.mod.* *.cmd .module* modules* Module* .*.cmd .tmp* *~

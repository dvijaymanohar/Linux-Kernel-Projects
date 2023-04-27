KBUILD_CFLAGS += -w

obj-m += lkm_netlink.o

all:
	make -w -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -Wall -g usa_netlink.c -o test

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -rf *.o *.ko *.mod.* *.cmd .module* modules* Module* .*.cmd .tmp* *~
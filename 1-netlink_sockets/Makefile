obj-m = netlink_lk_mod.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -Wall test.c -o test

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -rf modules.order Module.symvers sample_dev.ko sample_dev.mod.c sample_dev.mod.o sample_dev.o test

test:test.c
	cc -o test test.c

load:
	mknod /dev/sample_dev c 89 0
	insmod sample_dev.ko

unload:
	rmmod sample_dev.ko


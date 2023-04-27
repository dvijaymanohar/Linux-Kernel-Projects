KBUILD_CFLAGS += -w

obj-m := lk_nl_dev.o

lk_nl_dev-objs := lkm_nl.o lkm_nl_sys.o

all:
	make -w -C /lib/modules/$(shell uname -r)/build M=$(PWD) -I$(shell pwd) modules
	gcc -Wall -g us_app_nl.c -o test
	# rm -rf *.o  *.mod.* *.cmd .module* modules* Module* .*.cmd .tmp* *~

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -rf test *.o *.ko *.mod.* *.cmd .module* modules* Module* .*.cmd .tmp* *~

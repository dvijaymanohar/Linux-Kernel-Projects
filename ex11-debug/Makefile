DEBUG = n

ifeq ($(DEBUG),y)
	DEBFLAGS = -O -g -DHELLO_DEBUG
else
	DEBFLAGS = -O2
endif

CFLAGS += $(DEBFLAGS)	

obj-m	+= hello.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f *~
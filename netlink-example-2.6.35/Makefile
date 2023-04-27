KDIR := /lib/modules/$(shell uname -r)/build

#obj-m += nlexample.O
#obj-m += nlexample2.o
#obj-m += genlexample.o

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

userspace:
	#$(CC) -Wall -Wextra -Wno-unused -L/usr/lib/i386-linux-gnu/ -L/usr/lib -lmnl change.c -o change
	#$(CC) -Wall -Wextra -Wno-unused -lmnl event.c -o event
	#$(CC) -Wall -Wextra -Wno-unused -lmnl genl-change.c -o genl-change
	#$(CC) -Wall -Wextra -Wno-unused -lmnl genl-event.c -o genl-event
	#$(CC) -Wall -Wextra -Wno-unused -lmnl genl-resolve.c -o genl-resolve

	$(CC) -Wall -Wextra -Wno-unused -L/usr/lib/i386-linux-gnu/ -L/usr/lib change.c -lmnl -o change
	$(CC) -Wall -Wextra -Wno-unused event.c -lmnl -o event
	$(CC) -Wall -Wextra -Wno-unused genl-change.c -lmnl -o genl-change
	$(CC) -Wall -Wextra -Wno-unused genl-event.c -lmnl -o genl-event
	#$(CC) -Wall -Wextra -Wno-unused genl-resolve.c -lmnl -o genl-resolve

clean:
	rm -rf *~ *.o *.ko *.mod.* *.cmd .module* modules* Module* .*.cmd .tmp* change event

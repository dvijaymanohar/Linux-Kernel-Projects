KERNELDIR	?=/lib/modules/$(shell uname -r)/build

#KERNELDIR ?= /home/padmanabha/Linux/Kernel/kernel_2.6.10/linux-2.6.10_mvl401_LSP_01_30_00_082
#KERNELDIR ?= /home/padmanabha/Linux/Kernel/kernel_2.6.18/linux-2.6.18_pro500
#KERNELDIR  ?= /opt/mv_pro_5.0.0/montavista/pro/devkit/lsp/ti-davinci/linux-2.6.18_pro500

PWD := $(shell pwd)/src

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
#	$(CC) -o test_app src/test_app.c

#	mv src/*.ko test_app bin/
	mv src/*.ko bin/

	rm -rf src/*.o src/*.??* src/.*.cmd src/.tmp_versions

.PHONY=clean
clean:
	rm -rf bin/*



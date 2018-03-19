obj-m += cdev.o
#the absolute path 
KERNEL_SRC=`pwd`/../../../kernel #kernal source path

modules:
	$(MAKE) -C $(KERNEL_SRC) M=$(shell pwd) modules 
modules_install:
	$(MAKE)  $(KERNEL_SRC) M=$(shell pwd) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean

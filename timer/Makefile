# Makefile 4.1.15
ifneq ($(KERNELRELEASE),)

obj-m :=my_timer.o

else

KDIR = /home/scd/wintolinux/linux-imx-rel_imx_4.1.15_2.1.0_ga
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=`pwd` modules clean

endif

ifneq ($(KERNELRELEASE),)
obj-m += hello_platform_driver.o
else
KDIR := /home/lucasmonzonl/Documentos/mse/imd/buildroot/output/build/linux-rpi-6.6.y
all:
	$(MAKE) -C $(KDIR) M=$$PWD
endif

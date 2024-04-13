ifneq ($(KERNELRELEASE),)
obj-m += holamundo_i2c_driver.o
else
KDIR ?= /home/lucasmonzonl/Documentos/mse/imd/buildroot/output/build/linux-rpi-6.6.y
all:
	make -C $(KDIR) \
	ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
	M=${PWD}
endif

ifneq ($(KERNELRELEASE),)
obj-m += imu_i2c_driver.o
else
KDIR ?= /home/lucasmonzonl/Documentos/mse/imd/buildroot/output/build/linux-rpi-6.6.y
COMPILER_PATH = /home/lucasmonzonl/Documentos/mse/imd/buildroot/output/host/bin/
driver:
	make -C $(KDIR) \
	ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
	M=${PWD}

test:
	$(COMPILER_PATH)\$(CROSS_COMPILE)gcc imu_i2c_test.c -o imu_i2c_test
endif

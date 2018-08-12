obj-m += instrtmr.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

deploy:
	dmesg -C
	insmod instrtmr.ko

undeploy:
	rmmod instrtmr

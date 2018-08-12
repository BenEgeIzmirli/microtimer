obj-m += wbinvd.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

deploy:
	dmesg -C
	insmod wbinvd.ko

undeploy:
	rmmod wbinvd

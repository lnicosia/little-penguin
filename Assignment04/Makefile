obj-m += super-cool-keyboard-module.o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
		cp -v 42-keyboard.rules /etc/udev/rules.d/
		cp -v super-cool-keyboard-module.ko /tmp

usb:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
		cp -v 42-usb.rules /etc/udev/rules.d/
		cp -v super-cool-keyboard-module.ko /tmp

clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

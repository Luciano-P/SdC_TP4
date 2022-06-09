obj-m +=  drv_tp4.o 

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

c++:
	gcc -o test kern_test.c
	gcc -o read reader.c

s_c_t:
	sudo ./test

s_c_r:
	sudo ./read

s-python:
	sudo python3 plots_tp4.py
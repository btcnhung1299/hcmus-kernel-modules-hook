KDIR = /lib/modules/`uname -r`/build

all:
	make -C $(KDIR) M=`pwd`
	$(CC) RNG_user_space.c -o RNG_user_space
clean:
	make -C $(KDIR) M=`pwd` clean
	rm RNG_user_space

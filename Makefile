all:  bootloader kernel userland image

debug: all
	cd Kernel; make debug
	cd Userland; make debug

bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; make all

userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all

clean:
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

bear:
	bear -- make clean bootloader kernel; bear --append -- make userland

pvs-init:
	pvs-studio-analyzer credentials "PVS-Studio Free" "FREE-FREE-FREE-FREE"
	find . -name "*.c" -not -path '*/Bootloader/*' -prune -not -path '*/Toolchain/*' | while read line; do sed -i '1s/^\(.*\)$$/\/\/ This is a personal academic project. Dear PVS-Studio, please check it.\n\1/' "$$line"; done
	find . -name "*.c" -not -path '*/Bootloader/*' -prune -not -path '*/Toolchain/*' | while read line; do sed -i '2s/^\(.*\)$$/\/\/ PVS-Studio Static Code Analyzer for C, C++ and C#: http:\/\/www.viva64.com\n\1/' "$$line"; done

pvs:
	pvs-studio-analyzer trace -- make && pvs-studio-analyzer analyze -f strace_out && plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log


.PHONY: bootloader image collections kernel userland all clean

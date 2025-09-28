all: build/os.img

build/stage1.bin: boot/boot.asm
	nasm boot/boot.asm -f bin -o build/stage1.bin

build/stage2.o: kernel/stage2.asm
	nasm kernel/stage2.asm -f elf32 -o build/stage2.o

build/main.o: kernel/main.c
	gcc -m32 -ffreestanding -fno-pie -c kernel/main.c -o build/main.o

build/stage2_kernel.bin: build/stage2.o build/main.o kernel/linker.ld
	ld -m elf_i386 -T kernel/linker.ld -o build/stage2_kernel.bin build/stage2.o build/main.o

build/os.img: build/stage1.bin build/stage2_kernel.bin
	cat build/stage1.bin build/stage2_kernel.bin > build/os.img

run: build/os.img
	qemu-system-x86_64 -fda build/os.img

clr:
	rm -f build/*.o build/*.bin build/os.img


#Instaling features#
#updates#
get-nasm:
	sudo apt install nasm 
	
get-binutils:
	sudo apt install binutils
	
get-make:
	sudo apt install make

get-gcc:
	sudo apt install gcc-multilib g++-multilib

get-qemu:
	sudo apt install qemu qemu-system-x86

get-hexedit:
	sudo apt install wxhexeditor

get-update:
	sudo apt update

#run apps and more#
start-hexeditor:
	wxHexEditor

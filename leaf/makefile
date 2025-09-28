all:
	nasm -f bin boot/boot.asm -o build/boot.bin
	nasm -f bin kernel/main.asm -o build/main.bin
	dd if=/dev/zero of=build/os.img bs=512 count=2880
	dd if=build/boot.bin of=build/os.img bs=512 count=1 conv=notrunc
	dd if=build/main.bin of=build/os.img bs=512 seek=1 conv=notrunc


clr:
	rm build/boot.bin
	rm build/main.bin
	rm build/os.img

######################################################
#do this command if you dont have directories ! made !
######################################################
dirs:
	mkdir build
####coming soon#

run:
	qemu-system-i386 -fda build/os.img -boot a

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


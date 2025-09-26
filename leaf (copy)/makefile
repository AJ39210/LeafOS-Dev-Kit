all:
	nasm -f bin boot/boot.asm -o build/boot.bin
	nasm -f bin kernel/main.asm -o build/main.bin
	cat build/boot.bin build/main.bin > build/leafos.testing.img

clr:
	rm build/boot.bin
	rm build/main.bin
	rm build/leafos.testing.img

######################################################
#do this command if you dont have directories ! made !
######################################################
dirs:
	mkdir build
####coming soon#

run:
	qemu-system-i386 -drive format=raw,file=build/leafos.testing.img

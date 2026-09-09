.DEFAULT_GOAL := any
TARGET = BOOTX64.EFI

include uefi/Makefile

any: ${TARGET} intro move run

intro:
	@echo "Starting make process"

move:
	@echo "Moving file"
	mv ${TARGET} esp/EFI/BOOT/BOOTX64.EFI
run:
	 qemu-system-x86_64 -bios /usr/share/edk2/x64/OVMF.4m.fd -drive file=fat:rw:esp,format=raw
.DEFAULT_GOAL := all
TARGET=fos.efi

include uefi/Makefile

all: $(TARGET) intro move run

intro:
	@echo "Starting build process"
move:
	cp ${TARGET} esp/EFI/BOOT/BOOTX64.EFI
run:
	@echo "Running QEMU VM on ${TARGET}"
	@qemu-system-x86_64 -bios /usr/share/edk2/x64/OVMF.4m.fd -drive file=fat:rw:esp,format=raw



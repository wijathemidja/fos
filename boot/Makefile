.DEFAULT_GOAL := all
TARGET = BOOTX64.EFI

include uefi/Makefile
# the above line allows for the EFi to be made
all: ${TARGET} intro cleanup move run

intro:
	@echo ""
	@echo "Starting make process"
cleanup:
	@echo "Removing temp output files"
	@rm *.o
move:
	@mv ${TARGET} esp/EFI/BOOT/BOOTX64.EFI
	@echo "Generated ${TARGET} at esp/BOOT/BOOTX64.EFI"
run:
	@echo "Startng QEMU VM"
	@qemu-system-x86_64 -bios /usr/share/edk2/x64/OVMF.4m.fd -drive file=fat:rw:esp,format=raw
# this makes a virtual FAT drive at the esp folder containing the EFI. This lets QEMU boot the EFI
	@echo "Success. Exiting VM."
generate: ${TARGET} intro cleanup move



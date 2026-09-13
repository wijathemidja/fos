# fos
an operating sys


## Architecture

fos currently is planned to utilise a two step boot utlising UEFI.

im planning to have a very small simple bootloader (using POSIX-UEFI) to setup GOP and other protocols and load the kernel. then boot services are exited and the kernel takes control.

## Bootloader

currently to compile and run the fos bootloader in a QEMU vm, cd into the boot direcotry and run 'make'. The makefile is configured to use POSIX-UEFI's own makefile to compile and then moves the EFI into it's fake partition for QEMU.
make generate will just put the file in the fake esp partition and not run a QEMU VM if you want to use it it another device or actually use it.



thanks to bzt for developing POSIX-UEFI, which is used in the bootloader

## Project Structure

The bootloader can be found at boot folder
The kernel can be found at fosk folder

## Implemented 

Printing in bootloader 
Setting up GOP (with highest resolution available!)

# Use of AI

I highly dislike and discourage the use of AI in projects, especially 'vibecoding'. However it can sometimes be a useful tool.
I used AI to debug (diagnosing issues) and then wrote fixes completely by myself (and with a bit of help from StackOverflow adn the OSDev Wiki).
The only use of AI code in this project is the font, which was copied from a bitmap file. I had manually copied each byte individually, but for 96 characters that is just too much work.




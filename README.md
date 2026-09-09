# fos
an operating sys

fos currently is planned to utilise a two step boot utlising UEFI.

im planning to have a very small simple bootloader (using POSIX-UEFI) to setup GOP and other protocols and load the kernel. then boot services are exited and the kernel takes control.

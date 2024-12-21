gdb ./build/boot/PengOS.bin
target remote :1234
break kernel_main
continue
lay split
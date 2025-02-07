#!/bin/sh

# Build a disk image
DISK_IMAGE=disk.img
qemu-img create -f raw ${DISK_IMAGE} 100M

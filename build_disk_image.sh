#!/bin/sh

# Requires the `mtools` package.

# Build a disk image
DISK=disk.img
dd if=/dev/zero of=${DISK} bs=1M count=2
mformat -i ${DISK} -F
mcopy -i ${DISK} -s ./image/ ::/
mdir -i ${DISK} -/

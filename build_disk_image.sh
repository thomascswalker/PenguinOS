#!/bin/sh

# Requires the `mtools` package.

# Build a disk image
DISK=disk.img
SIZE=32
dd if=/dev/zero of=${DISK} bs=${SIZE}M count=1
mformat -i ${DISK} -F
mcopy -i ${DISK} -s ./base/ ::/
mdir -i ${DISK} -/

#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/chimpos.kernel isodir/boot/chimpos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "chimp-os" {
	multiboot /boot/chimpos.kernel
}
EOF
grub-mkrescue -o chimpos.iso isodir

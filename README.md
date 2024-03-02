# Chimp OS
A primitive 32 bit operating system. Written in C.

Targeting x86 i386 architecture.

Builds with Binutils & GCC (i386-elf-gcc) cross-compiler toolchain.

Boots with GRUB, runs in QEMU and on hardware.

Currently iterating on kernel design and userspace while implementing a standard library for functionality.

This is a recreational programming project.


https://github.com/cameronslee/chimp-os/assets/29127398/641ffa6f-ff45-4534-b636-d5c3ea4c21fd



## Requirements
- i386-elf cross compiler (GCC)
- Linker and Assembler (Binutils)
- QEMU
- GRUB bootloader

## Usage
```
$ ./clean.sh                    # clears out object & binaries for new build
$ ./headers.sh                  # reads in headers
$ ./iso.sh                      # generate .iso
$ ./qemu.sh                     # run!
```

## Kernel Implementation
This is a list of what has been implemented so far. I will update as often as possible.
- Supports Multiboot via GRUB
- Paging (Virtual Memory Space) 
- Interrupt Service Routines (ISRs)
- Interrupt Requests (IRQs)
- VGA Graphics
- Teletype Terminal (TTY)
- Programmable Interval Timer (PIT) - handles system uptime
- Keyboard Handler (keyboard hardware IRQs, (IRQ1))
- Standard Library (growing!)
- Global Descriptor Table (GDT) & Interrupt Descriptor Table (IDT)
- Stack Smashing Protector (SSP) - detect stack buffer overrun

### Design Notes
libc and libk
libc: hosted library, required. 

libk: freestanding library that does not contain any code specific to user space 



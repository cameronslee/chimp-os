
ENTRY (_start)

/*                                                                           */
/*  Kernel Start   multiboot                                     Kernel End  */
/*   0x00100000     headers                                      0xC0000000  */
/*         ^           ^                                           ^         */
/*         |    -------|                                           |         */
/*         |    |                                                  |         */
/*    -------------------------------------------------------------------    */
/*    |    x    x                                                  x    |    */
/*    -------------------------------------------------------------------    */
/*         |                                                                 */
/*         v                                                                 */
/*   Kernel lives at 3GB + 1MB                                               */
/*   in virtual address space                                                */
/*   Virtual address space mapped                                            */
/*   to mapped to 1MB of physical                                            */ 
/*   address space                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* Page-align the sections on a 4KB boundary. (Address in multiple of 4KB)   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
SECTIONS
{
        . = 0x00100000;

	kernel_start = .;
        .multiboot.data : {
            *(.multiboot.data)
        }

       .multiboot.text : {
           *(.multiboot.text)
       }

	. += 0xC0000000;

	/* text: executable code */
	.text ALIGN (4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.text)
	}

    /* rodata: read only data, constants, immutable data */
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}

    /* data: initialized data, global variables */
	.data ALIGN (4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}

    /* data: uninitialized data*/
	.bss ALIGN (4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}

	/* Add a symbol that indicates the end address of the kernel. */
	kernel_end = .;
}

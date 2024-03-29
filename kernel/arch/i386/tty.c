#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>
#include <kernel/pit.h>

#include "vga.h"

#include <limits.h> // INT_MIN and INT_MAX

/*
 * SCROLLBACK HANDLE
 * 
 * Alloc enough space for ONE terminal screen in the 
 * terminal buffer. (VGA_HEIGHT * VGA_WIDTH)
 * 
 * Upon term scrolling we alloc more space dynamically 
 *
 * TODO implement a memory manager!!!
*/
#define MAX_SCROLLBACK 10000 // DEFAULT!
#define BACKSPACE 0x08 
#define TAB 0x09 

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

// 1024th page reservered for VGA text buffer - see boot.S
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xC03FF000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

unsigned int curr_fg_color;
unsigned int curr_bg_color;

void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;

    // Set colors to draw
    curr_fg_color = VGA_COLOR_WHITE;
    curr_bg_color = VGA_COLOR_CYAN;
	terminal_color = vga_entry_color(curr_fg_color, curr_bg_color);

	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

/* reset cursor positions and clear terminal buffer */
void terminal_clear()
{
	terminal_row = 0;
	terminal_column = 0;

	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
	unsigned char uc = c;
    
    // backspace
    if (c == BACKSPACE && terminal_column != 0) {
        terminal_column -= 1;
        // "remove" char from buffer
        uc = ' ';
        terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
        terminal_column -= 1;
    }
    // tab: inc col to a point that is divisible by 8
    else if (c == TAB) {
        terminal_column = (terminal_column + 8) & ~(8 - 1);
    }
    // carriage return 
    else if (c == '\r') {
        terminal_column = 0;
    }
    // newline handle, behave like <CR> and then move down a row
    else if (c == '\n') {
        terminal_column = 0;
        terminal_row += 1;
    }
    // we print the character
    else if (c >= ' ') {
        terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
    }
    
    // TODO handle scrollback buffering
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}

void splash_screen() {
    unsigned int spacing_y = 2;
    unsigned int spacing_x = VGA_WIDTH / 2;
    const char* d;
    const char* msg;

    // print border
    d = "=";
    for (int i = 0; i < VGA_WIDTH; i++) 
        terminal_writestring((const char *) d);
    
    // space y
    d = "\n";
    for (int i = 0; i < spacing_y; i++) 
        terminal_writestring((const char *) d);
    
    // space x
    msg  = "Welcome to Chimp OS!";
    d = " ";
    for (int i = 0; i < spacing_x - (strlen(msg) / 2); i++) 
        terminal_writestring((const char *) d);
    
    terminal_writestring((const char *) msg);

    // space y
    d = "\n";
    for (int i = 0; i < spacing_y; i++) 
        terminal_writestring((const char *) d);

    const char *banana[] = {
        "   //\\",
        "   V  \\",
        "    \\  \\_",
        "     \\,'.`-.",
        "      |\\ `. `.       ",
        "      ( \\  `. `-.                        _,.-:\\",
        "       \\ \\   `.  `-._             __..--' ,-';/",
        "        \\ `.   `-.   `-..___..---'   _.--' ,'/",
        "         `. `.    `-._        __..--'    ,' /",
        "           `. `-_     ``--..''       _.-' ,'",
        "             `-_ `-.___        __,--'   ,'",
        "                `-.__  `----\\\"\"\"\"__.-'",
        "                   `--..____..--'"
    };

    for (int i = 0; i < sizeof(banana) / sizeof(banana[0]); i++) {
        int spaces = (35 - strlen(banana[0])) / 2;
        for (int j = 0; j < spaces; j++) {
            
            terminal_writestring((const char *)" ");
        }
        terminal_writestring((const char *) banana[i]);
        terminal_writestring((const char *)"\n");
    }

    // space y
    d = "\n";
    for (int i = 0; i < spacing_y; i++) 
        terminal_writestring((const char *) d);

    // space x
    msg = "*****"; // full loading bar
    d = " ";
    for (int i = 0; i < spacing_x - (strlen(msg) / 2) - 1; i++) 
        terminal_writestring((const char *) d);

    // "loading bar" - FIXME sync to RTC for loading additonal resources
    int loading_bar_index = 0;
    int loading_bar_time = 500; // 500 ms

    while (loading_bar_index < loading_bar_time) {
        terminal_writestring((const char *)"* ");
        loading_bar_index += 100;
        timer_wait(25);
    }

    terminal_clear();
}

// TODO, read PS1 from a shell config when implemented. (need VFS)
void terminal_prompt(const char *usr, const char *device_name, const char *curr_dir) {
    terminal_writestring((const char *) usr);
    terminal_writestring((const char *)"@");
    terminal_writestring((const char *) device_name);
    terminal_writestring((const char *)" ");
    terminal_writestring((const char *) curr_dir);
    terminal_writestring((const char *)"$ ");
}

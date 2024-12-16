#if defined(__linux__)
#error "You are not using a cross-compiler."
#endif

#if !defined(__i386__)
// #error "This needs to be compiled with ix86-elf."
#endif

/* Hardware text mode color constants. */
enum vga_color
{
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

typedef enum vga_color vga_color_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;

// VGA

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static inline uint8_t make_entry_color(vga_color_t fore_color, vga_color_t back_color)
{
    return fore_color | back_color << 4;
}

static inline uint16_t create_entry(unsigned char character, uint8_t color)
{
    return (uint16_t)character | (uint16_t)color << 8;
}

// String

const char CHAR_NEW_LINE = '\n';
const char CHAR_TAB = '\t';

uint32_t strlen(const char *string)
{
    uint32_t length = 0;
    while (string[length])
    {
        length++;
    }
    return length;
}

// Terminal

uint16_t *g_terminal_buf; // Buffer of all terminal text.
uint32_t g_terminal_row;  // The current cursor row.
uint32_t g_terminal_col;  // The current cursor column.
uint8_t g_terminal_color; // The current cursor color.

void init_terminal(void)
{
    g_terminal_row = 0;
    g_terminal_col = 0;
    g_terminal_color = make_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    g_terminal_buf = (uint16_t *)0xB8000;
    for (uint32_t y = 0; y < VGA_HEIGHT; y++)
    {
        for (uint32_t x = 0; x < VGA_WIDTH; x++)
        {
            const uint32_t index = y * VGA_WIDTH + x;
            g_terminal_buf[index] = create_entry(' ', g_terminal_color);
        }
    }
}

void set_terminal_color(uint8_t color)
{
    g_terminal_color = color;
}

void put_terminal(char c)
{
    // Handle new lines
    if (c == CHAR_NEW_LINE)
    {
        g_terminal_row++;
        g_terminal_col = 0;
        return;
    }

    const uint32_t index = g_terminal_row * VGA_WIDTH + g_terminal_col;
    g_terminal_buf[index] = create_entry(c, g_terminal_color);
    if (++g_terminal_col == VGA_WIDTH)
    {
        g_terminal_col = 0;
        if (++g_terminal_row == VGA_HEIGHT)
        {
            g_terminal_row = 0;
        }
    }
}

void write_terminal(const char *data, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        put_terminal(data[i]);
    }
}

void printf(const char *data)
{
    write_terminal(data, strlen(data));
}

void printfc(const char *data, vga_color_t color)
{
    // Store the current color as previous.
    vga_color_t prev_color = g_terminal_color;
    // Temporarily set the terminal color to the specified color.
    set_terminal_color(color);
    // Write the text to the terminal.
    write_terminal(data, strlen(data));
    // Reset the terminal color back to the previous color.
    set_terminal_color(prev_color);
}

void kernel_main(void)
{
    init_terminal();
    printfc("Welcome to PengOS.\n", VGA_COLOR_LIGHT_GREEN);
    printf("Thomas Walker\n");
}

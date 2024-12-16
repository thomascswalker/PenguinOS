#if defined(__linux__)
#error "You are not using a cross-compiler."
#endif

#if !defined(__i386__)
// #error "This needs to be compiled with ix86-elf."
#endif

/* Hardware text mode color constants. */
enum VGAColor
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

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;

const char CHAR_NEW_LINE = '\n';
const char CHAR_TAB = '\t';

static inline uint8_t VGA_EntryColor(enum VGAColor foreColor, enum VGAColor backColor)
{
    return foreColor | backColor << 4;
}

static inline uint16_t VGA_Entry(unsigned char character, uint8_t color)
{
    return (uint16_t)character | (uint16_t)color << 8;
}

uint32_t String_Length(const char *string)
{
    uint32_t length = 0;
    while (string[length])
    {
        length++;
    }
    return length;
}

static const uint32_t g_VGA_WIDTH = 80;
static const uint32_t g_VGA_HEIGHT = 25;

uint32_t g_terminalRow;
uint32_t g_terminalColumn;
uint8_t g_terminalColor;
uint16_t *g_terminalBuffer;

void Terminal_Initialize(void)
{
    g_terminalRow = 0;
    g_terminalColumn = 0;
    g_terminalColor = VGA_EntryColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    g_terminalBuffer = (uint16_t *)0xB8000;
    for (uint32_t y = 0; y < g_VGA_HEIGHT; y++)
    {
        for (uint32_t x = 0; x < g_VGA_WIDTH; x++)
        {
            const uint32_t index = y * g_VGA_WIDTH + x;
            g_terminalBuffer[index] = VGA_Entry(' ', g_terminalColor);
        }
    }
}

void Terminal_SetColor(uint8_t color)
{
    g_terminalColor = color;
}

void Terminal_PutEntryAt(char c, uint8_t color, uint32_t x, uint32_t y)
{
    const uint32_t index = y * g_VGA_WIDTH + x;
    g_terminalBuffer[index] = VGA_Entry(c, color);
}

void Terminal_PutChar(char c)
{
    if (c == CHAR_NEW_LINE)
    {
        g_terminalRow++;
        g_terminalColumn = 0;
        return;
    }
    Terminal_PutEntryAt(c, g_terminalColor, g_terminalColumn, g_terminalRow);
    if (++g_terminalColumn == g_VGA_WIDTH)
    {
        g_terminalColumn = 0;
        if (++g_terminalRow == g_VGA_HEIGHT)
        {
            g_terminalRow = 0;
        }
    }
}

void Terminal_WriteString(const char *data, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        Terminal_PutChar(data[i]);
    }
}

void Terminal_PrintF(const char *data)
{
    Terminal_WriteString(data, String_Length(data));
}

void Kernel_Main(void)
{
    /* Initialize terminal interface */
    Terminal_Initialize();

    /* Newline support is left as an exercise. */
    Terminal_PrintF("Welcome to PengOS.\nTest new line.\n");
}

#include <io.c>
#include <keyboard.h>

enum scancode
{
	SC_ESC = 0X01,			// Esc
	SC_1 = 0x02,			// 1
	SC_2 = 0x03,			// 2
	SC_3 = 0x04,			// 3
	SC_4 = 0x05,			// 4
	SC_5 = 0x06,			// 5
	SC_6 = 0x07,			// 6
	SC_7 = 0x08,			// 7
	SC_8 = 0x09,			// 8
	SC_9 = 0x0A,			// 9
	SC_0 = 0x0B,			// 0
	SC_MINUS = 0x0C,		// -
	SC_EQUALS = 0x0D,		// =
	SC_BACKSPACE = 0x0E,	// Backspace
	SC_TAB = 0x0F,			// Tab
	SC_Q = 0x10,			// q
	SC_W = 0x11,			// w
	SC_E = 0x12,			// e
	SC_R = 0x13,			// r
	SC_T = 0x14,			// t
	SC_Y = 0x15,			// y
	SC_U = 0x16,			// u
	SC_I = 0x17,			// i
	SC_O = 0x18,			// o
	SC_P = 0x19,			// p
	SC_BRACKETLEFT = 0x1A,	// [
	SC_BRACKETRIGHT = 0x1B, // ]
	SC_ENTER = 0x1C,		// Enter
	SC_CONTROLLEFT = 0x1D,	// Left Control
	SC_A = 0x1E,			// a
	SC_S = 0x1F,			// s
	SC_D = 0x20,			// d
	SC_F = 0x21,			// f
	SC_G = 0x22,			// g
	SC_H = 0x23,			// h
	SC_J = 0x24,			// j
	SC_K = 0x25,			// k
	SC_L = 0x26,			// l
	SC_SEMICOLON = 0x27,	// ;
	SC_APOSTROPHE = 0x28,	// '
	SC_GRAVE = 0x29,		// Return
	SC_SHIFTLEFT = 0x2A,	// Left Shift
	SC_BACKSLASH = 0x2B,	// '\'
	SC_Z = 0x2C,			// z
	SC_X = 0x2D,			// x
	SC_C = 0x2E,			// c
	SC_V = 0x2F,			// v
	SC_B = 0x30,			// b
	SC_N = 0x31,			// n
	SC_M = 0x32,			// m
	SC_COMMA = 0x33,		// ,
	SC_PREIOD = 0x34,		// .
	SC_SLASH = 0x35,		// /
	SC_SHIFTRIGHT = 0x36,	// Right Shift
};
typedef enum scancode scancode_t;

struct keycode
{
	scancode_t code;
	char	   lower;
	char	   upper;
};
typedef struct keycode keycode_t;

static keycode_t keycodes[10] = {
	{ SC_1, '1', '!' },
	{ SC_2, '2', '@' },
	{ SC_3, '3', '#' },
	{ SC_4, '4', '$' },
	{ SC_5, '5', '%' },
	{ SC_6, '6', '^' },
	{ SC_7, '7', '&' },
	{ SC_8, '8', '*' },
	{ SC_9, '9', '(' },
	{ SC_0, '0', ')' },
};

void init_keyboard()
{
	register_interrupt_handler(IRQ1, keyboard_callback);
}

bool get_keycode(scancode_t sc, keycode_t* kc)
{
	for (int32_t i = 0; i < 10; i++)
	{
		if (keycodes[i].code == sc)
		{
			*kc = keycodes[i];
			return true;
		}
	}
	return false;
}

void keyboard_callback(registers_t regs)
{
	uint8_t sc = inb(0x60);
	if (sc & 0x80)
	{
		return;
	}
	keycode_t kc;
	if (get_keycode(sc, &kc))
	{
		info("A key was pressed: %c", kc.lower);
	}
	else
	{
		warning("An unknown key was pressed: %i", sc);
	}
}

#include <keyboard.h>
#include <pic.h>
#include <stdio.h>

// Tracks the state of any modifier keys (Shift, Alt, Ctrl)
static modifier_key_t g_modifier_key;
static bool			  g_shift_down = false;

void init_keyboard()
{
	debug("Initializing keyboard...");
	while (inb(KEYBOARD_STATUS_PORT) & KEYBOARD_INPUT_FULL)
	{
	}
	register_interrupt_handler(IRQ1, keyboard_callback);
	success("Keyboard initialized.");
}

bool get_keycode(scancode_t sc, keycode_t* kc)
{
	uint32_t keymap_count = sizeof(keymap) / sizeof(keycode_t);
	for (uint32_t i = 0; i < keymap_count; i++)
	{
		if (keymap[i].code == sc)
		{
			*kc = keymap[i];
			return true;
		}
	}
	return false;
}

void keyboard_callback(registers_t regs)
{
	uint8_t	  sc = inb(KEYBOARD_DATA_PORT); // Incoming scancode
	keycode_t kc;							// Converted keyboard code struct

	if (sc & 0x80) // Released
	{
		sc -= 128; // Mask release bit
		if (get_keycode(sc, &kc))
		{
			on_key_released(&kc);
		}
	}
	else if (get_keycode(sc, &kc)) // Pressed
	{
		on_key_pressed(&kc);
	}
}

void on_key_released(keycode_t* kc)
{
	switch (kc->code)
	{
		case SC_SHIFTLEFT:
		case SC_SHIFTRIGHT:
			{
				g_shift_down = false;
				return;
			}
		case SC_ESC:
			{
				exit();
			}
		default:
			return;
	}
}

void on_key_pressed(keycode_t* kc)
{
	switch (kc->code)
	{
		case SC_ESC:
			{
				exit();
				return;
			}
		case SC_SHIFTLEFT:
		case SC_SHIFTRIGHT:
			{
				g_shift_down = true;
				return;
			}
		case SC_ENTER:
			{
				terminal_new_line();
				return;
			}
		case SC_BACKSPACE:
			{
				remchar();
				update_cursor_pos();
				return;
			}
		default:
			break;
	}

	char c = is_shift_down() ? kc->upper : kc->lower;
	putchar(c);
	update_cursor_pos();
}

bool is_shift_down()
{
	return g_shift_down;
}

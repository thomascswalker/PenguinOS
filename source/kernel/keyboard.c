#include <keyboard.h>
#include <pic.h>
#include <stdio.h>

static modifier_key_t g_modifier_key;
static bool			  g_left_shift_down = false;
static bool			  g_right_shift_down = false;

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
	for (int32_t i = 0; i < 128; i++)
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

	if (get_keycode(sc, &kc))
	{
		if (sc & 0x80) // Released
		{
			kc.code -= 128; // Remove the shift modifier
			on_key_released(&kc);
		}
		else // Pressed
		{
			on_key_pressed(&kc);
		}
	}
	pic_send_eoi(IRQ1);
}

void on_key_released(keycode_t* kc)
{
	switch (kc->code)
	{
		case SC_SHIFTLEFT:
		case SC_SHIFTRIGHT:
			{
				g_modifier_key &= ~(MOD_SHIFT);
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
				return;
			}
		case SC_SHIFTLEFT:
		case SC_SHIFTRIGHT:
			{
				g_modifier_key |= MOD_SHIFT;
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
	return g_modifier_key & MOD_SHIFT;
}

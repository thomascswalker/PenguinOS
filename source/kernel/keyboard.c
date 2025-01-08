#include <keyboard.h>
#include <pic.h>
#include <stdio.h>

// Tracks the state of any modifier keys (Shift, Alt, Ctrl)
static modifier_key_t g_modifier_key;

void init_keyboard()
{
	debug("Initializing keyboard...");

	// Wait for the keyboard to be available.
	while (inb(KEYBOARD_STATUS_PORT) & KEYBOARD_INPUT_FULL)
	{
	}

	// Register IRQ1 (ISR33) to execute our keyboard callback
	// function.
	register_interrupt_handler(IRQ1, keyboard_callback);
	success("Keyboard initialized.");
}

// Given the input scancode, retrieve the keycode struct
// containing `{ scancode, lower, upper }`.
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

// Called when IRQ1 (ISR33) is executed.
void keyboard_callback(registers_t regs)
{
	uint8_t	  sc = inb(KEYBOARD_DATA_PORT); // Incoming scancode
	keycode_t kc;							// Converted keyboard code struct

	/*
	Check if the input scancode has the 8th bit set to 1.
	If it does, this indicates the key is being released.
	Otherwise, the key is being pressed.

	---------------------------------
	| 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
	---------------------------------
	  ^ 1 if released, 0 if pressed

	The remaining 7 bits dictate which key was actually
	pressed.
	*/
	if (sc & 0x80) // Released
	{
		sc &= ~(0x80); // Set bit 8 to zero.
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
				g_modifier_key &= ~MOD_SHIFT;
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

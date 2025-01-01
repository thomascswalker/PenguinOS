#include <io.c>
#include <keyboard.h>

static bool g_left_shift_down = false;
static bool g_right_shift_down = false;

void init_keyboard()
{
	register_interrupt_handler(IRQ1, keyboard_callback);
}

bool get_keycode(scancode_t sc, keycode_t* kc)
{
	for (int32_t i = 0; i < 128; i++)
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
	uint8_t	  sc = inb(0x60); // Incoming scancode
	keycode_t kc;			  // Converted keyboard code struct

	if (get_keycode(sc, &kc))
	{
		// debug("Keycode: %i", kc.code);
		if (sc & 0x80)
		{
			kc.code -= 128;
			on_key_released(&kc);
		}
		else
		{
			on_key_pressed(&kc);
		}
		return;
	}
}

void on_key_released(keycode_t* kc)
{
	if (kc->code == SC_SHIFTLEFT)
	{
		g_left_shift_down = false;
	}
	if (kc->code == SC_SHIFTRIGHT)
	{
		g_right_shift_down = false;
	}
}

void on_key_pressed(keycode_t* kc)
{
	if (kc->code == SC_SHIFTLEFT)
	{
		g_left_shift_down = true;
		return;
	}
	else if (kc->code == SC_SHIFTRIGHT)
	{
		g_right_shift_down = true;
		return;
	}

	char c = is_shift_down() ? kc->upper : kc->lower;
	putchar(c);
	update_cursor_pos();
}

bool is_shift_down()
{
	return g_left_shift_down || g_right_shift_down;
}

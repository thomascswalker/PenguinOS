#pragma once

#include <idt.h>

void init_keyboard();
void keyboard_callback(registers_t regs);

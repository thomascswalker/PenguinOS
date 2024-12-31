#pragma once
#include <stdint.h>

#define PIC0_COMMAND 0x20
#define PIC1_COMMAND 0xA0
#define PIC0_DATA (PIC0_COMMAND + 1)
#define PIC1_DATA (PIC1_COMMAND + 1)
#define PIC_EOI 0x20

static void pic_irq_mask(uint8_t index);
static void pic_irq_unmask(uint8_t index);
static void pic_remap();
static void pic_send_eoi(uint8_t index);

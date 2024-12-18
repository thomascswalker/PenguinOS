#pragma once

extern unsigned char inportb(unsigned short _port)
{
	unsigned char rv;
	asm("inb %1, %0" : "=a"(rv) : "dN"(_port));
	return rv;
};

extern void outportb(unsigned short _port, unsigned char _data)
{
	asm("outb %1, %0" : : "dN"(_port), "a"(_data));
}
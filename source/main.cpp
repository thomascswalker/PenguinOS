/*
Main entry point into PengOS.
*/

#include <gdt.h>
#include <keyboard.h>
#include <memory.h>
#include <multiboot.h>
#include <pit.h>
#include <string.h>

EXTERN void kmain(MultibootInfo* info, uint32_t magic)
{
	VGA::init();
	println("Initializing PengOS...");
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		panic("Invalid Multiboot magic value.");
	}
	GDT::init();
	IDT::init();
	PIT::init();
	Keyboard::init();

	// Once everything is initialized, enable interrupts.
	enableInterrupts();
	println("Welcome to PengOS!");

	uint32_t start = 0;
	uint32_t size = 0;
	Multiboot::init(info, &start, &size);
	Memory::init(start, size);

	std::String str1("String1");
	std::String str2("String2");
	std::String str3 = str1 + str2;
	std::String str4("String4");
	std::String str5 = "String5";
	str4.append(str3);
	str4.prepend("lskjdflkjsdf");
	str4.append("jjjjjjjjjj");

	debugs(str1);
	debugs(str2);
	debugs(str3);
	debugs(str4);
	debugs(str5);

	while (1)
	{
	}
}
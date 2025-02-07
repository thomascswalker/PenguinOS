#include <ata.h>
#include <stdio.h>

ATADevice primaryMaster;
ATADevice primarySlave;
ATADevice secondaryMaster;
ATADevice secondarySlave;

void ATADevice::init(bool inPrimary, bool inMaster)
{
	drive = inMaster;
	channel = inPrimary;
	uint16_t base = channel ? 0x1F0 : 0x170;

	ports.data = base;
	ports.err = base + 1;
	ports.sectorCount = base + 2;
	ports.lbaLow = base + 3;
	ports.lbaMid = base + 4;
	ports.lbaHigh = base + 5;
	ports.device = base + 6;
	ports.command = base + 7;
	ports.control = channel ? 0x3F6 : 0x376;
}

void IDE::init()
{
	primaryMaster.init(true, true);
	primarySlave.init(true, false);
	secondaryMaster.init(false, true);
	secondarySlave.init(false, false);

	primaryMaster.identify();
	primarySlave.identify();
	secondaryMaster.identify();
	secondarySlave.identify();
}

void ATADevice::wait()
{
	for (uint32_t i = 0; i < 4; i++)
	{
		inb(ports.control);
	}
}

void ATADevice::reset()
{
	outb(ports.control, 0x4);
	wait();
	outb(ports.control, 0x0);
}

void ATADevice::select()
{
	if (drive)
	{
		outb(ports.device, 0xA0);
	}
	else
	{
		outb(ports.device, 0xB0);
	}
}

void ATADevice::identify()
{
	reset();
	select();

	wait();

	outb(ports.sectorCount, 0);
	outb(ports.lbaLow, 0);
	outb(ports.lbaMid, 0);
	outb(ports.lbaHigh, 0);

	outb(ports.command, ATA_CMD_IDENTIFY);

	uint8_t status = inb(ports.command);
	if (status != 0x58)
	{
		return;
	}
	uint8_t mask = 1 << 7;
	while (status & mask)
	{
		status = inb(ports.command);
	}
	debug("Status: %x", status);

	uint8_t drq = 0, err = 0;
	while (!drq && !err)
	{
		drq = inb(ports.command) & 0x8;
		err = inb(ports.command) & 0x0;
	}
	if (err)
	{
		panic("Error when polling.");
	}

	uint16_t buffer[256];
	memset(buffer, 0, 256);
	for (int i = 0; i < 256; i++)
	{
		buffer[i] = inw(ports.data);
	}

	// Bit 15 of the first word of the IDENTIFY buffer
	// determines whether the device is a valid ATA device.
	if ((buffer[0] & 0x40) != 0x40)
	{
		panic("ATA Device not recognized.");
	}

	// 0b000_0100_0000_0000
	lbaExtSupported = (buffer[83] & 0x400) == 0x400;
	if (lbaExtSupported)
	{
		debug("48-bit Address");
	}
	else
	{
		debug("28-bit Address");
	}

	serial = buffer[ATA_IDENT_SERIAL];
	debug("Serial: %d", serial);

	sectorCount = *((uint32_t*)(buffer + ATA_IDENT_MAX_LBA));
	debug("Sector count: %d", sectorCount);
	debug("Drive size is %dMB", size() / 1024 / 1024);

	uint8_t* modelPtr = (uint8_t*)(buffer + ATA_IDENT_MODEL);
	/*
		ATA strings are weird and are written in 16bit increments,
		where the second 8bits are the first character and the
		first 8bits are the second character.

		-------------------------
		| Char 1 | Char 3 | ... |
		| Char 0 | Char 2 | ... |
		-------------------------
	*/

	for (uint32_t i = 0; i < 21; i++)
	{
		auto offset = i * 2;
		model[offset] = modelPtr[offset + 1];
		model[offset + 1] = modelPtr[offset];
	}
	model[40] = 0; // Terminate string at the last byte
	debug("Drive model: %s", model);
}

#pragma once

#include <stdio.h>

typedef struct cpuid
{
	char	 manufacturer_id[12];
	uint32_t family_id;
	uint32_t model;
	uint32_t processor_type;
} cpuid_t;

typedef enum cpuid_requests
{
	CPUID_MANUFACTURER_ID,
	CPUID_FEATURES,
} cpuid_requests_t;

void cpuid(cpuid_requests_t code, uint8_t subcode, uint32_t* out)
{
	__asm__("cpuid"
			: "=a"(out[0]), "=b"(out[1]), "=c"(out[2]), "=d"(out[3]) // Output operands
			: "a"(code), "c"(subcode)								 // Input operands
	);
}

void get_manufacturer(cpuid_t* cpu)
{
	uint32_t buffer[12];
	uint32_t regs[4];
	cpuid(CPUID_MANUFACTURER_ID, 0, regs);
	buffer[0] = regs[1]; // ebx
	buffer[1] = regs[3]; // edx
	buffer[2] = regs[2]; // ecs
	strcpy(cpu->manufacturer_id, (char*)buffer);
}

void get_processor_info(cpuid_t* cpu)
{
	uint32_t regs[4];
	cpuid(CPUID_FEATURES, 0, regs);
}

void get_cpuid(cpuid_t* cpu)
{
	// EAX 0
	get_manufacturer(cpu);

	// EAX 1
	get_processor_info(cpu);
}

void print_cpuid()
{
	cpuid_t cpu;
	get_cpuid(&cpu);

	printf("Processor: %s\n", cpu.manufacturer_id);
}
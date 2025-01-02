#define cpuid(in, a, b, c, d) __asm__("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(in));

#include <string.h>

void detect_cpu(char* str)
{ /* or main() if your trying to port this as an independant application */
	unsigned long ebx, unused;
	cpuid(0, unused, ebx, unused, unused);
	const char* intel = "Intel";
	const char* amd = "amd";
	const char* unknown = "unknown";
	switch (ebx)
	{
		case 0x756e6547: /* Intel Magic Code */
			strcpy(str, intel);
			break;
		case 0x68747541: /* AMD Magic Code */
			strcpy(str, amd);
			break;
		default:
			strcpy(str, unknown);
			break;
	}
}
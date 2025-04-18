#include <cstdlib.h>
#include <cstring.h>

/**
 * Copies one buffer to another.
 *
 * @param dest Destination buffer.
 * @param source Source buffer.
 * @param size Number of bytes to copy.
 *
 * @return Pointer to the destination buffer.
 */
void* memcpy(void* dest, void* source, size_t size)
{
	uint8_t*	   p_dest = (uint8_t*)dest;
	const uint8_t* p_source = (uint8_t*)source;

	while (size--)
	{
		*p_dest++ = *p_source++;
	}
	return dest;
}

/**
 * Fills a buffer with a repeated uint8_t value.
 *
 * @param dest Destination buffer.
 * @param value Value to fill the buffer with.
 * @param size [n] bytes to fill.
 *
 * @return Pointer to the destination buffer.
 */
void* memset(void* dest, uint8_t value, int32_t size)
{
	char* tmp = (char*)dest;
	for (int32_t i = 0; i < size; i++)
	{
		tmp[i] = value;
	}
	return tmp;
}

/**
 * Fills a buffer with a repeated uint16_t value.
 *
 * @param dest Destination buffer.
 * @param value Value to fill the buffer with.
 * @param size [n * 2] bytes to fill.
 *
 * @return Pointer to the destination buffer.
 */
uint16_t* wmemset(uint16_t* dest, uint16_t value, int32_t size)
{
	for (int32_t i = 0; i < size; i++)
	{
		dest[i] = value;
	}
	return dest;
}

/**
 * Returns the length of the string.
 * @param string The string to measure.
 *
 * @return The length of the string.
 */
size_t strlen(const char* string)
{
	size_t length = 0;
	while (string[length])
	{
		length++;
	}
	return length;
}

/**
 * Compares two strings.
 *
 * @param lhs The first string to compare.
 * @param rhs The second string to compare.
 *
 * @return `true` if the strings are equal, `false`
 * otherwise.
 */
bool strcmp(const char* lhs, const char* rhs)
{
	if (strlen(lhs) != strlen(rhs))
	{
		return false;
	}

	for (size_t i = 0; i < strlen(lhs); i++)
	{
		if (lhs[i] != rhs[i])
		{
			return false;
		}
	}

	return true;
}

bool strncmp(const char* lhs, const char* rhs, size_t count)
{
	if (strlen(lhs) < count || strlen(rhs) < count)
	{
		return false;
	}

	for (size_t i = 0; i < count; i++)
	{
		if (lhs[i] != rhs[i])
		{
			return false;
		}
	}

	return true;
}

// Copies one string to another
char* strcpy(char* dest, const char* source)
{
	char* temp = dest;
	while ((*dest++ = *source++))
	{
	}
	return temp;
}

EXTERN char* strncpy(char* dest, const char* source, size_t count)
{
	char*  temp = dest;
	size_t i = 0;
	while ((*dest++ = *source++) && i < count)
	{
		i++;
	}
	return temp;
}

// Appends one string to another
char* strcat(char* dest, const char* source)
{
	size_t i = 0, j = 0;
	while (dest[i] != '\0')
	{
		i++;
	}
	while (dest[j] != '\0')
	{
		dest[i + j] = source[j];
		j++;
	}
	dest[i + j] = '\0';

	return dest;
}

EXTERN char* strchr(const char* str, char c)
{
	// Loop until the null terminator is reached.
	while (*str)
	{
		if (*str == c)
		{
			return const_cast<char*>(str);
		}
		str++;
	}
	// If searching for the null terminator, return pointer to it.
	return (c == '\0') ? const_cast<char*>(str) : nullptr;
}

EXTERN int32_t strchri(const char* s, char c)
{
	char* pos = strchr(s, c);
	if (pos == nullptr)
	{
		return -1;
	}
	return pos - s;
}

EXTERN char* strrchr(const char* str, char c)
{
	const char* last = nullptr;
	// Loop through the string.
	while (*str)
	{
		if (*str == c)
		{
			last = str;
		}
		str++;
	}
	// If searching for the null terminator, return pointer to it.
	return (c == '\0') ? const_cast<char*>(str) : const_cast<char*>(last);
}

EXTERN int32_t strrchri(const char* s, char c)
{
	char* pos = strrchr(s, c);
	if (pos == nullptr)
	{
		return -1;
	}
	return pos - s;
}

char* strdup(const char* str)
{
	size_t size = strlen(str) + 1;
	char*  copy = (char*)malloc(size);
	if (copy)
	{
		memcpy(copy, const_cast<char*>(str), size);
	}
	return copy;
}

char* strtok(char* str, const char delim)
{
	static size_t pos = 0;
	static char*  buffer = nullptr;
	static char*  token = nullptr;

	if (str)
	{
		pos = 0;
		buffer = str;

		if (token)
		{
			delete token;
		}
		token = new char[strlen(buffer) + 1];
	}

	if (!buffer)
	{
		return nullptr;
	}

	if (pos >= strlen(buffer))
	{
		return nullptr;
	}

	memset(token, 0, strlen(buffer) + 1);

	size_t i = 0;
	while (pos < strlen(buffer))
	{
		if (buffer[pos] == delim)
		{
			pos++;
			return token;
		}
		token[i] = buffer[pos];
		i++;
		pos++;
	}
	return token;
}

char toupper(char c)
{
	if (!islower(c))
	{
		return c;
	}
	return c - 'a' + 'A';
}

char tolower(char c)
{
	if (!isupper(c))
	{
		return c;
	}
	return c - 'A' + 'a';
}

// Reverses, in-place, the specified `string`.
void strrev(char* string)
{
	int start = 0;
	int end = strlen(string) - 1;
	while (start < end)
	{
		char c = string[start];
		string[start] = string[end];
		string[end] = c;

		start++;
		end--;
	}
}

void strapp(char* source, const char* string, char* dest)
{
	strcpy(dest, source);
	dest += sizeof(source);
	strcpy(dest, string);
}

void strprep(char* source, const char* string, char* dest)
{
	strcpy(dest, string);
	dest += sizeof(string);
	strcpy(dest, source);
}

void wtoc(char* dest, wchar_t* source, size_t length)
{
	uint32_t i = 0;
	while (length--)
	{
		*dest++ = source[i];
		i += 2;
	}
}
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

/**
 * Compares up to `count` characters of two strings.
 *
 * @param str1 The first string to compare.
 * @param str2 The second string to compare.
 * @param count The number of characters to compare.
 *
 * @return `true` if the strings are equal, `false`
 * otherwise.
 */
bool strncmp(const char* str1, const char* str2, size_t count)
{
	if (strlen(str1) < count || strlen(str2) < count)
	{
		return false;
	}

	for (size_t i = 0; i < count; i++)
	{
		if (str1[i] != str2[i])
		{
			return false;
		}
	}

	return true;
}

/**
 * Copies one string to another.
 *
 * @param dest Pointer to the destination array where the
 * content is to be copied.
 * @param source String to be copied.
 *
 * @return Pointer to the destination string.
 */
char* strcpy(char* dest, const char* source)
{
	char* temp = dest;
	while ((*dest++ = *source++))
	{
	}
	return temp;
}

/**
 * Copies up to `count` characters from one string to another.
 *
 * @param dest Pointer to the destination array where the
 * content is to be copied.
 * @param source String to be copied.
 * @param count The maximum number of characters to copy.
 *
 * @return Pointer to the destination string.
 */
char* strncpy(char* dest, const char* source, size_t count)
{
	char*  temp = dest;
	size_t i = 0;
	while ((*dest++ = *source++) && i < count)
	{
		i++;
	}
	return temp;
}

/**
 * Appends a copy of the source string to the destination
 * string. The terminating null character in destination is
 * overwritten by the first character of source, and a
 * null-character is included at the end of the new string
 * formed by the concatenation of both in destination.
 *
 * @param dest Pointer to the destination array where the
 * content is to be appended.
 * @param source String to be appended.
 *
 * @return Pointer to the destination string.
 */
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

/**
 * Locates the first occurrence of the character `c` in the
 * string and returns a pointer to it.
 *
 * @param str The string to search.
 * @param c The character to search for.
 *
 * @return Pointer to the located character or a null
 * pointer.
 */
char* strchr(const char* str, char c)
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

/**
 * Locates the first occurrence of the character `c` in the
 * string and returns its index.
 *
 * @param str The string to search.
 * @param c The character to search for.
 *
 * @return Index of the located character or -1 if not found.
 */
int32_t strchri(const char* s, char c)
{
	char* pos = strchr(s, c);
	if (pos == nullptr)
	{
		return -1;
	}
	return pos - s;
}

/**
 * Locates the last occurrence of the character `c` in the
 * string and returns a pointer to it.
 *
 * @param str The string to search.
 * @param c The character to search for.
 *
 * @return Pointer to the located character or a null
 * pointer.
 */
char* strrchr(const char* str, char c)
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

/**
 * Locates the last occurrence of the character `c` in the
 * string and returns its index.
 *
 * @param str The string to search.
 * @param c The character to search for.
 *
 * @return Index of the located character or -1 if not found.
 */
int32_t strrchri(const char* s, char c)
{
	char* pos = strrchr(s, c);
	if (pos == nullptr)
	{
		return -1;
	}
	return pos - s;
}

/**
 * Duplicates a string by allocating memory for it and copying
 * the content.
 *
 * @param str The string to duplicate.
 *
 * @return Pointer to the duplicated string or a null pointer.
 */
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

/**
 * Tokenizes a string using the specified delimiter.
 *
 * @param str The string to tokenize.
 * @param delim The delimiter character.
 *
 * @return Pointer to the next token or a null pointer.
 */
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

/**
 * Converts a character to uppercase.
 *
 * @param c The character to convert.
 *
 * @return The uppercase character or the original character
 * if it is not lowercase.
 */
char toupper(char c)
{
	if (!islower(c))
	{
		return c;
	}
	return c - 'a' + 'A';
}

/**
 * Converts a character to lowercase.
 *
 * @param c The character to convert.
 *
 * @return The lowercase character or the original character
 * if it is not uppercase.
 */
char tolower(char c)
{
	if (!isupper(c))
	{
		return c;
	}
	return c - 'A' + 'a';
}

/**
 * Reverses, in-place, the specified `string`.
 *
 * @param string The string to reverse.
 */
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

/**
 * Converts a wide character string to a narrow character
 * string.
 *
 * @param dest Pointer to the destination array where the
 * content is to be copied.
 * @param source Wide character string to be converted.
 * @param length The number of wide characters to convert.
 */
void wtoc(char* dest, wchar_t* source, size_t length)
{
	uint32_t i = 0;
	while (length--)
	{
		*dest++ = source[i];
		i += 2;
	}
}

/**
 * Checks if a string starts with a given prefix.
 *
 * @param str The string to check.
 * @param prefix The prefix to check for.
 *
 * @return `true` if the string starts with the prefix,
 * `false` otherwise.
 */
bool startswith(const char* str, const char* prefix)
{
	int32_t len = strlen(prefix);
	return strncmp(str, prefix, len);
}

/**
 * Checks if a string ends with a given suffix.
 *
 * @param str The string to check.
 * @param suffix The suffix to check for.
 *
 * @return `true` if the string ends with the suffix,
 * `false` otherwise.
 */
bool endswith(const char* str, const char* suffix)
{
	int32_t len = strlen(suffix);
	return strncmp(str + strlen(str) - len, suffix, len);
}

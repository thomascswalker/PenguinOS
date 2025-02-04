#include <string.h>

namespace std
{
	String::String(const char* str)
	{
		m_data = (char*)std::kmalloc(strlen(str));
		strcpy(m_data, str);
	}

	String::String(size_t size)
	{
		m_data = (char*)std::kmalloc(size);
		memset(m_data, 0, strlen(m_data));
	}

	String::String(size_t size, char c)
	{
		m_data = (char*)std::kmalloc(size);
		memset(m_data, c, strlen(m_data));
	}

	char*  String::data() const { return m_data; }
	size_t String::size() const { return strlen(m_data); }
	void   String::append(const String& other) { *this += other; }
	void   String::append(const char* other) { *this += String(other); }
	void   String::prepend(String& other) { *this = other + *this; }
	void   String::prepend(const char* other) { *this = String(other) + *this; }

	/* Operators */

	String String::operator+(const String& other)
	{
		size_t newSize = size() + other.size();
		String newStr(newSize);
		strcpy(newStr.data(), data());
		strcpy(newStr.data() + size(), other.data());
		return newStr;
	}
	String& String::operator+=(const String& other)
	{
		strcpy(data() + size(), other.data());
		return *this;
	}
	String::operator char*() const { return m_data; }
} // namespace std
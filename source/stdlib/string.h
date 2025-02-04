#include <cstring.h>
#include <memory.h>

namespace std
{
	class String
	{
	private:
		char* m_data;

	public:
		/* Constructors */

		String() = default;
		String(const char* str)
		{
			m_data = (char*)std::kmalloc(strlen(str));
			strcpy(m_data, str);
		}
		String(size_t size)
		{
			m_data = (char*)std::kmalloc(size);
			memset(m_data, 0, strlen(m_data));
		}
		String(size_t size, char c)
		{
			m_data = (char*)std::kmalloc(size);
			memset(m_data, c, strlen(m_data));
		}
		~String() = default;

		/* Methods */

		char*  data() const { return m_data; }
		size_t size() const { return strlen(m_data); }
		void   append(const String& other) { *this += other; }
		void   append(const char* other) { *this += String(other); }
		void   prepend(String& other) { *this = other + *this; }
		void   prepend(const char* other) { *this = String(other) + *this; }

		/* Operators */

		String operator+(const String& other)
		{
			size_t newSize = size() + other.size();
			String newStr(newSize);
			strcpy(newStr.data(), data());
			strcpy(newStr.data() + size(), other.data());
			return newStr;
		}
		String& operator+=(const String& other)
		{
			strcpy(data() + size(), other.data());
			return *this;
		}
		operator char*() const { return m_data; }
	};
} // namespace std
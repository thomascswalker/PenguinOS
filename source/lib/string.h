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
		String(const char* str);
		String(size_t size);
		String(size_t size, char c);
		~String() = default;

		/* Methods */

		char*  data() const;
		size_t size() const;
		void   append(const String& other);
		void   append(const char* other);
		void   prepend(String& other);
		void   prepend(const char* other);

		/* Operators */

		String	operator+(const String& other);
		String& operator+=(const String& other);
		operator char*() const;
	};
} // namespace std
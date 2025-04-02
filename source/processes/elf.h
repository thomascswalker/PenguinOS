#pragma once

#include <filesystem.h>
#include <stdint.h>

#define EI_MAG0 0
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_OSABI 7
#define EI_ABIVERSION 8
#define EI_PAD 9
#define EI_TYPE 10
#define EI_MACHINE 12
#define EI_NIDENT 16

namespace ELF
{
	struct FileHeader
	{
		uint8_t	 ident[16];				   // ELF identification
		uint16_t type;					   // Object file type
		uint16_t machine;				   // Machine type
		uint32_t version;				   // Object file version
		uint32_t entry;					   // Entry point address
		uint32_t programHeaderOffset;	   // Program header offset
		uint32_t sectionHeaderOffset;	   // Section header offset
		uint32_t flags;					   // Processor-specific flags
		uint16_t entryHeaderSize;		   // ELF header size
		uint16_t programHeaderSize;		   // Size of program header entry
		uint16_t programHeaderCount;	   // Number of program headers
		uint16_t sectionHeaderSize;		   // Size of section header entry
		uint16_t sectionHeaderCount;	   // Number of section headers
		uint16_t sectionHeaderStringIndex; // Section name string table index

		bool	is32Bit() const { return ident[EI_CLASS] == 1; }
		uint8_t getABI() const { return ident[EI_OSABI]; }
		bool	isLittleEndian() const
		{
			return ident[EI_DATA] == 1; // 1 = little endian, 2 = big endian
		}

		void dump() const;
	};

	struct ProgramHeader
	{
		uint32_t type;	 // Type of segment
		uint32_t offset; // Offset in file
		uint32_t vaddr;	 // Virtual address in memory
		uint32_t paddr;	 // Physical address (not used)
		uint32_t filesz; // Size of segment in file
		uint32_t memsz;	 // Size of segment in memory
		uint32_t flags;	 // Segment attributes
		uint32_t align;	 // Alignment of segment

		void dump() const;
	};

	struct SectionHeader
	{
		uint32_t name;		// Section name (index into string table)
		uint32_t type;		// Section type
		uint32_t flags;		// Section attributes
		uint32_t addr;		// Address in memory
		uint32_t offset;	// Offset in file
		uint32_t size;		// Size of section in bytes
		uint32_t link;		// Link to another section (if applicable)
		uint32_t info;		// Additional section information
		uint32_t addralign; // Address alignment constraints
		uint32_t entsize;	// Size of entries (if section has entries)

		void dump(const char* inName) const;
	};

	class ELFFile
	{
		File*				 m_file;
		FileHeader			 m_header;
		Array<ProgramHeader> m_programHeaders;
		Array<SectionHeader> m_sectionHeaders;
		Array<String>		 m_sectionNames;

	public:
		ELFFile(const char* path);
		~ELFFile() {}

		bool isValid() const { return m_file != nullptr; }
	};

} // namespace ELF
#include <elf.h>

using namespace ELF;

ELFFile::ELFFile(const char* path)
{
	m_file = new File();
	if (FileSystem::openFile(path, m_file))
	{
		parseHeaders();
		dumpHeaders();
	}
	else
	{
		warning("ELF: Failed to open file: '%s'", path);
		m_file = nullptr;
	}
}

void ELF::ELFFile::parseHeaders()
{
	char* data = (char*)m_file->data;

	// Read the main ELF header
	memcpy(&m_header, data, sizeof(FileHeader));

	// Read all program headers, the count being defined
	// by FileHeader.phnum
	for (uint32_t i = 0; i < m_header.programHeaderCount; i++)
	{
		ProgramHeader ph;
		memcpy(&ph, data + m_header.programHeaderOffset + (i * sizeof(ProgramHeader)),
			sizeof(ProgramHeader));

		// Add to program header array
		m_programHeaders.add(ph);
	}

	// Read all section headers, the count being defined
	// by FileHeader.shnum
	for (uint32_t i = 0; i < m_header.sectionHeaderCount; i++)
	{
		SectionHeader sh;
		memcpy(&sh, data + m_header.sectionHeaderOffset + (i * sizeof(SectionHeader)),
			sizeof(SectionHeader));

		// Add to program header array
		m_sectionHeaders.add(sh);
	}

	auto nameHeader = m_sectionHeaders[m_header.sectionHeaderStringIndex];
	auto nameData = new char[nameHeader.size];
	memcpy(nameData, data + nameHeader.offset, nameHeader.size);

	m_sectionNames = String(nameData, nameHeader.size).split('\0');
}

void ELF::ELFFile::dumpHeaders() const
{
	m_header.dump();
	for (const auto& header : m_programHeaders)
	{
		header.dump();
	}
	for (size_t i = 0; i < m_sectionHeaders.size(); i++)
	{
		m_sectionHeaders[i].dump(m_sectionNames[i].data());
	}
}

void ELF::FileHeader::dump() const
{
	printf("ELF Header:\n"
		   "  Type: %d Machine: %d Version: %d\n"
		   "  Entry point: %x\n"
		   "  Program header offset: %x\n"
		   "  Section header offset: %x\n"
		   "  Flags: %x\n"
		   "  ELF header size: %d\n"
		   "  Program header entry size: %d\n"
		   "  Number of program headers: %d\n"
		   "  Section header entry size: %d\n"
		   "  Number of section headers: %d\n"
		   "  Section name string table index: %d\n",
		type, machine, version, entry, programHeaderOffset, sectionHeaderOffset, flags,
		entryHeaderSize, programHeaderSize, programHeaderCount, sectionHeaderSize,
		sectionHeaderCount, sectionHeaderStringIndex);
}

void ELF::ProgramHeader::dump() const
{
	printf("Program Header:\n"
		   "  Type: %d             Offset: %x    Virtual Address: %x\n"
		   "  Physical Address: %x File Size: %x Memory Size: %x\n"
		   "  Flags: %x            Alignment: %x\n",
		type, offset, vaddr, paddr, filesz, memsz, flags, align);
}

void ELF::SectionHeader::dump(const char* inName) const
{
	printf("Section %s:\n"
		   "  NameIndex: %d Type: %d Flags: %x\n"
		   "  Address: %x Offset: %x Size: %x\n"
		   "  Link: %d Info: %d Address Align: %x Entry Size: %x\n",
		inName, name, type, flags, addr, offset, size, link, info, addralign, entsize);
}

#include <elf.h>

using namespace ELF;

ELFFile::ELFFile(const char* path)
{
	// m_file = new File();
	// if (FileSystem::openFile(path, m_file))
	// {
	// 	parseHeaders();
	// 	dumpHeaders();
	// }
	// else
	// {
	// 	warning("ELF: Failed to open file: '%s'", path);
	// 	m_file = nullptr;
	// }
}

void ELF::ELFFile::parseHeaders()
{
	// char* data = (char*)m_file->data;

	// // Read the main ELF header
	// memcpy(&m_header, data, sizeof(FileHeader));

	// // Read all program headers, the count being defined
	// // by FileHeader.phnum
	// for (uint32_t i = 0; i < m_header.programHeaderCount; i++)
	// {
	// 	ProgramHeader header;
	// 	memcpy(&header, data + m_header.programHeaderOffset + (i * sizeof(ProgramHeader)),
	// 		sizeof(ProgramHeader));

	// 	// Add to program header array
	// 	m_programHeaders.add(header);
	// }

	// // Read all section headers, the count being defined
	// // by FileHeader.shnum
	// for (uint32_t i = 0; i < m_header.sectionHeaderCount; i++)
	// {
	// 	SectionHeader header;
	// 	memcpy(&header, data + m_header.sectionHeaderOffset + (i * sizeof(SectionHeader)),
	// 		sizeof(SectionHeader));

	// 	// Add to program header array
	// 	m_sectionHeaders.add(header);
	// }

	// auto nameHeader = m_sectionHeaders[m_header.sectionHeaderStringIndex];
	// auto nameData = new char[nameHeader.size];
	// memcpy(nameData, data + nameHeader.offset, nameHeader.size);

	// m_sectionNames = String(nameData, nameHeader.size).split('\0');
}

void ELF::ELFFile::dumpHeaders() const
{
	m_header.dump();
	printf("Program Headers:\n");
	printf("  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align\n");
	for (const auto& header : m_programHeaders)
	{
		header.dump();
	}
	printf("Section Headers:\n");
	printf("  [Nr] Name         Type   Addr       Off      Size     ES Flg Lk Inf Al\n");
	for (size_t i = 0; i < m_sectionHeaders.size(); i++)
	{
		m_sectionHeaders[i].dump(m_sectionNames[i].data(), i);
	}
}

void ELF::FileHeader::dump() const
{
	printf("ELF Header:\n"
		   "  Type:                            %d\n"
		   "  Machine:                         %d\n"
		   "  Version:                         %d\n"
		   "  Entry point:                     %x\n"
		   "  Program header offset:           %x\n"
		   "  Section header offset:           %x\n"
		   "  Flags:                           %x\n"
		   "  ELF header size:                 %d\n"
		   "  Program header entry size:       %d\n"
		   "  Number of program headers:       %d\n"
		   "  Section header entry size:       %d\n"
		   "  Number of section headers:       %d\n"
		   "  Section name string table index: %d\n",
		type, machine, version, entry, programHeaderOffset, sectionHeaderOffset, flags,
		entryHeaderSize, programHeaderSize, programHeaderCount, sectionHeaderSize,
		sectionHeaderCount, sectionHeaderStringIndex);
}

void ELF::ProgramHeader::dump() const
{

	printf("  %-14d %06x %08x %08x %05x %05x %03d %-04x\n", type, offset, vaddr, paddr, filesz,
		memsz, flags, align);
}

void ELF::SectionHeader::dump(const char* inName, int index) const
{
	if (!inName)
	{
		return;
	}
	printf("  [%2d] %-12s %-6d %08x %06x %06x %02d %03d %d  %d   %d\n", index, inName, type, addr,
		offset, size, entsize, flags, link, info, addralign);
}

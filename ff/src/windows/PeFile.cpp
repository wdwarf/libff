#include <ff/windows/PeFile.h>

NS_FF_BEG

PeFile::PeFile()
    : m_fileHandle(nullptr), m_fileMap(nullptr), m_fileMem(nullptr) {}

PeFile::~PeFile() { this->close(); }

bool PeFile::open(const std::string &filePath) {
  m_fileHandle =
      CreateFileA(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

  if (INVALID_HANDLE_VALUE == m_fileHandle || nullptr == m_fileHandle)
    return false;

  m_fileMap =
      CreateFileMapping(m_fileHandle, nullptr, PAGE_READONLY, 0, 0, nullptr);
  if (nullptr == m_fileMap) {
    this->close();
    return false;
  }

  m_fileMem = MapViewOfFile(m_fileMap, FILE_MAP_READ, 0, 0, 0);
  if (nullptr == m_fileMem) {
    this->close();
    return false;
  }

  return true;
}

void PeFile::close() {
  if (nullptr != m_fileMem) UnmapViewOfFile(m_fileMem);

  if (nullptr != m_fileMap) CloseHandle(m_fileMap);

  if (nullptr != m_fileMem && INVALID_HANDLE_VALUE != m_fileMem)
    CloseHandle(m_fileMem);
}

template <class ImageNtHeader>
static inline uint32_t NtHeaderWordSize(const ImageNtHeader *header) {
  return IMAGE_NT_OPTIONAL_HDR32_MAGIC == header->OptionalHeader.Magic   ? 32
         : IMAGE_NT_OPTIONAL_HDR64_MAGIC == header->OptionalHeader.Magic ? 64
                                                                         : 0;
}

const IMAGE_NT_HEADERS *PeFile::getNtHeader() const {
  IMAGE_DOS_HEADER *dosHeader = static_cast<PIMAGE_DOS_HEADER>(m_fileMem);
  if (IsBadReadPtr(dosHeader, sizeof(IMAGE_DOS_HEADER)) ||
      dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
    return nullptr;
  }

  char *ntHeaderC = static_cast<char *>(m_fileMem) + dosHeader->e_lfanew;
  IMAGE_NT_HEADERS *ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS *>(ntHeaderC);

  if (IsBadReadPtr(ntHeaders, sizeof(ntHeaders->Signature)) ||
      ntHeaders->Signature != IMAGE_NT_SIGNATURE ||
      IsBadReadPtr(&ntHeaders->FileHeader, sizeof(IMAGE_FILE_HEADER))) {
    return nullptr;
  }

  if (0 == NtHeaderWordSize(ntHeaders)) {
    return nullptr;
  }

  IMAGE_SECTION_HEADER *sectionHeaders = IMAGE_FIRST_SECTION(ntHeaders);
  if (IsBadReadPtr(sectionHeaders, ntHeaders->FileHeader.NumberOfSections *
                                       sizeof(IMAGE_SECTION_HEADER))) {
    return nullptr;
  }

  return ntHeaders;
}

NS_FF_END

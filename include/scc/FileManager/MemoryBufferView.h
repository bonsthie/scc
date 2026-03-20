#ifndef SCC_FILEMANAGER_MEMORYBUFFERVIEW_H
#define SCC_FILEMANAGER_MEMORYBUFFERVIEW_H

#include <fcntl.h>
#include <string_view>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace scc {

struct MemoryViewPos {
    unsigned Buff = 0;
    unsigned Line = 1;
    unsigned Column = 1;

    MemoryViewPos() = default;
    MemoryViewPos(unsigned Line, unsigned Column) : Line(Line), Column(Column) {}
    MemoryViewPos(const MemoryViewPos &) = default;
    MemoryViewPos &operator=(const MemoryViewPos &) = default;
};

class MemoryBufferView {
    const char *Data = nullptr;
    size_t      Size = 0;

  public:
    explicit MemoryBufferView(const char *Data, size_t Size) noexcept : Data(Data), Size(Size) {}
    MemoryBufferView(MemoryBufferView &&o) noexcept;
    MemoryBufferView(MemoryBufferView &o) noexcept;
    ~MemoryBufferView() = default;

    MemoryBufferView &operator=(MemoryBufferView &o) noexcept;
    MemoryBufferView &operator=(MemoryBufferView &&o) noexcept;

	const char &operator[](const size_t Idx) const { return Data[Idx]; }

	std::string_view getStringView(const MemoryViewPos &Begin, const MemoryViewPos &End) const;


    size_t size() const { return Size; }
    const char *raw() const { return Data; }
	const char *end() const { return Data + Size; }
    char operator[](int idx) { return Data[idx]; }
};

} // namespace scc

#endif // SCC_FILEMANAGER_MEMORYBUFFERVIEW_H

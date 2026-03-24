#ifndef SCC_FILEMANAGER_MEMORYBUFFERVIEW_H
#define SCC_FILEMANAGER_MEMORYBUFFERVIEW_H

#include "scc/FileManager/FileID.h"
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

struct PosView {
    MemoryViewPos P;
    const FileID *FID{nullptr};

    void print(std::ostream &OS) const;
};

std::ostream &operator<<(std::ostream &OS, const PosView &V);

struct SourceRange {
    const FileID *FID{nullptr};
    MemoryViewPos Begin;
    MemoryViewPos End;

    SourceRange() = default;
    SourceRange(FileID *FID, MemoryViewPos Begin, MemoryViewPos End)
        : FID(FID),
          Begin(Begin),
          End(End) {}

    PosView posViewBegin() const { return {Begin, FID}; }
    PosView posViewEnd() const { return {End, FID}; }

    bool isValid() const { return Begin.Line != 0 && End.Line != 0; }
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

    size_t      size() const { return Size; }
    const char *raw() const { return Data; }
    const char *end() const { return Data + Size; }
    char        operator[](int idx) { return Data[idx]; }
};

} // namespace scc

#endif // SCC_FILEMANAGER_MEMORYBUFFERVIEW_H

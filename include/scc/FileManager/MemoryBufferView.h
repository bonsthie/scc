#ifndef SCC_FILEMANAGER_MEMORYBUFFERVIEW_H
#define SCC_FILEMANAGER_MEMORYBUFFERVIEW_H

#include <algorithm>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace scc {

class MemoryBufferView {
    const char *Data = nullptr;
    size_t      Size = 0;

  public:
    explicit MemoryBufferView(const char *Data, size_t Size) : Data(Data), Size(Size) {}
    MemoryBufferView(MemoryBufferView &&o) noexcept;
    ~MemoryBufferView() = default;

    MemoryBufferView &operator=(MemoryBufferView &&o) noexcept {
        if (this != &o) {
            this->~MemoryBufferView();
            new (this) MemoryBufferView(std::move(o));
        }
        return *this;
    }

    size_t size() const { return Size; }

    const char *raw() const { return Data; }

    char operator[](int idx) { return Data[idx]; }
};

} // namespace scc

#endif // SCC_FILEMANAGER_MEMORYBUFFERVIEW_H

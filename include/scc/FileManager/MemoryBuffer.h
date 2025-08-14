#ifndef SCC_FILEMANAGER_MEMORYBUFFER_H
#define SCC_FILEMANAGER_MEMORYBUFFER_H

#include <cstdint>
#include <fcntl.h>
#include <stdexcept>
#include <string_view>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace scc {

class MemoryBuffer {
    void  *_data = nullptr;
    size_t _size = 0;
    int    _fd = -1;

  public:
    explicit MemoryBuffer(char const *path);
	MemoryBuffer(MemoryBuffer &&o) noexcept;
    ~MemoryBuffer();


    MemoryBuffer &operator=(MemoryBuffer &&o) noexcept {
        if (this != &o) {
            this->~MemoryBuffer();
            new (this) MemoryBuffer(std::move(o));
        }
        return *this;
    }

    size_t size() const { return _size; }

    std::string_view view() const { return {static_cast<char const *>(_data), _size}; }

    void *raw() const { return _data; }

    operator std::string_view() const { return view(); }
};

} // namespace scc

#endif // SCC_FILEMANAGER_MEMORYBUFFER_H

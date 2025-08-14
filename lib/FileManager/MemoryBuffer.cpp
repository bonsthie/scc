#include "scc/FileManager/MemoryBuffer.h"


using namespace scc;

MemoryBuffer::MemoryBuffer(char const *path) {
    _fd = ::open(path, O_RDONLY);
    if (_fd < 0)
        throw std::runtime_error("open failed");

    struct stat st;
    if (fstat(_fd, &st) < 0) {
        ::close(_fd);
        throw std::runtime_error("stat failed");
    }
    _size = st.st_size;

    _data = mmap(nullptr, _size, PROT_READ, MAP_PRIVATE, _fd, 0);
    if (_data == MAP_FAILED) {
        ::close(_fd);
        throw std::runtime_error("mmap failed");
    }
}

MemoryBuffer::~MemoryBuffer() {
    if (_data && _data != MAP_FAILED)
        munmap(_data, _size);
    if (_fd >= 0)
        close(_fd);
}

MemoryBuffer::MemoryBuffer(MemoryBuffer &&o) noexcept : _data(o._data), _size(o._size), _fd(o._fd) {
    o._data = nullptr;
    o._fd = -1;
    o._size = 0;
}


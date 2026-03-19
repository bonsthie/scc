#ifndef SCC_ALLOCATOR_BUMP_ALLOCATOR_H
#define SCC_ALLOCATOR_BUMP_ALLOCATOR_H

#include <cstddef>
#include <memory>
#include <vector>

namespace scc {

class BumpAllocator {
  public:
    explicit BumpAllocator(size_t chunk_size = 1024 * 1024) : chunkSize(chunk_size) {
        allocChunk();
    }

    BumpAllocator(const BumpAllocator &) = delete;
    BumpAllocator &operator=(const BumpAllocator &) = delete;

    template <typename T, typename... Args> T *alloc(Args &&...args) {
        void *ptr = rawAlloc(sizeof(T), alignof(T));
        return new (ptr) T(std::forward<Args>(args)...);
    }

    void reset() {
        for (auto &chunk : chunks) {
            chunk.offset = 0;
        }
        currentChunkIdx = 0;
    }

  private:
    struct Chunk {
        std::unique_ptr<std::byte[]> data;
        size_t                       offset = 0;
    };

    void *rawAlloc(size_t size, size_t alignment);
    void  allocChunk();

    std::vector<Chunk> chunks;
    size_t             currentChunkIdx = 0;
    size_t             chunkSize;
};

} // namespace scc

#endif // SCC_ALLOCATOR_BUMP_ALLOCATOR_H

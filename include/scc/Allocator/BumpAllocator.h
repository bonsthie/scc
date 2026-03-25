#ifndef SCC_ALLOCATOR_BUMP_ALLOCATOR_H
#define SCC_ALLOCATOR_BUMP_ALLOCATOR_H

#include <cstddef>
#include <memory>
#include <vector>

#include "scc/Allocator/Allocator.h"

namespace scc {

class BumpAllocator : public Allocator {
  public:
    explicit BumpAllocator(size_t chunk_size = 1024 * 1024) : chunkSize(chunk_size) {
        allocChunk();
    }

    BumpAllocator(const BumpAllocator &) = delete;
    BumpAllocator &operator=(const BumpAllocator &) = delete;

    void reset() override {
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

    void *allocate_bytes(size_t size, size_t alignment = alignof(std::max_align_t)) override;
    void  allocChunk();

    std::vector<Chunk> chunks;
    size_t             currentChunkIdx = 0;
    size_t             chunkSize;
};

} // namespace scc

#endif // SCC_ALLOCATOR_BUMP_ALLOCATOR_H

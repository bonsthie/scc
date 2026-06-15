#ifndef SCC_ALLOCATOR_BUMPALLOCATOR_H
#define SCC_ALLOCATOR_BUMPALLOCATOR_H

#include <cstddef>
#include <memory>

#include "scc/ADT/vector.h"
#include "scc/Allocator/Allocator.h"

namespace scc {

class BumpAllocator : public Allocator {
  public:
    explicit BumpAllocator(size_t ChunkSize = 1024 * 1024) : ChunkSize(ChunkSize) {
        allocChunk();
    }

    BumpAllocator(const BumpAllocator &) = delete;
    BumpAllocator &operator=(const BumpAllocator &) = delete;

    void reset() override {
        for (auto &Chunk : Chunks) {
            Chunk.Offset = 0;
        }
        CurrentChunkIdx = 0;
    }

    void *allocateBytes(size_t Size, size_t Alignment = alignof(std::max_align_t)) override;

  private:
    struct Chunk {
        std::unique_ptr<std::byte[]> Data;
        size_t                       Offset = 0;
    };

    void  allocChunk();

    scc::Vector<Chunk> Chunks;
    size_t             CurrentChunkIdx = 0;
    size_t             ChunkSize;
};

} // namespace scc

#endif // SCC_ALLOCATOR_BUMPALLOCATOR_H

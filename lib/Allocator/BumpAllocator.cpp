
#include "scc/Allocator/BumpAllocator.h"
#include <cstddef>
#include <memory>

using namespace scc;

void *BumpAllocator::rawAlloc(size_t size, size_t alignment) {
    auto  &chunk = chunks[currentChunkIdx];
    void  *ptr = chunk.data.get() + chunk.offset;
    size_t space = chunkSize - chunk.offset;

    if (std::align(alignment, size, ptr, space)) {
        chunk.offset = chunkSize - space + size;
        return ptr;
    }

    // If it doesn't fit, allocate a new chunk and try again
    allocChunk();
    return rawAlloc(size, alignment);
}

void BumpAllocator::allocChunk() {
    if (currentChunkIdx + 1 < chunks.size()) {
        ++currentChunkIdx;
        chunks[currentChunkIdx].offset = 0;
        return;
    }

    auto data = std::make_unique<std::byte[]>(chunkSize);
    chunks.push_back({.data = std::move(data), .offset = 0});
    currentChunkIdx = chunks.size() - 1;
}

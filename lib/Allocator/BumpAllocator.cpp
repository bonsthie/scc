
#include "scc/Allocator/BumpAllocator.h"
#include <cstddef>
#include <memory>

using namespace scc;

void *BumpAllocator::allocateBytes(size_t Size, size_t Alignment) {
    auto  &Chunk = Chunks[CurrentChunkIdx];
    void  *Ptr = Chunk.Data.get() + Chunk.Offset;
    size_t Space = ChunkSize - Chunk.Offset;

    if (std::align(Alignment, Size, Ptr, Space)) {
        Chunk.Offset = ChunkSize - Space + Size;
        return Ptr;
    }

    // If it doesn't fit, allocate a new chunk and try again
    allocChunk();
    return allocateBytes(Size, Alignment);
}

void BumpAllocator::allocChunk() {
    if (CurrentChunkIdx + 1 < Chunks.size()) {
        ++CurrentChunkIdx;
        Chunks[CurrentChunkIdx].Offset = 0;
        return;
    }

    auto Data = std::make_unique<std::byte[]>(ChunkSize);
    Chunks.pushBack({.Data = std::move(Data), .Offset = 0});
    CurrentChunkIdx = Chunks.size() - 1;
}

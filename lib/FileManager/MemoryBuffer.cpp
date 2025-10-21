#include "scc/FileManager/MemoryBufferView.h"

using namespace scc;

MemoryBufferView::MemoryBufferView(MemoryBufferView &&o) noexcept { *this = std::move(o); }

MemoryBufferView::MemoryBufferView(MemoryBufferView &o) noexcept { *this = o; }

MemoryBufferView &MemoryBufferView::operator=(MemoryBufferView &&o) noexcept {
    if (this != &o) {
        Data = std::move(o.Data);
        Size = o.Size;
    }
    return *this;
}

MemoryBufferView &MemoryBufferView::operator=(MemoryBufferView &o) noexcept {
    if (this != &o) {
        Data = o.Data;
        Size = o.Size;
    }
    return *this;
}

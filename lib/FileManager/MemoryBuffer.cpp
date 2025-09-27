#include "scc/FileManager/MemoryBufferView.h"

using namespace scc;

MemoryBufferView::MemoryBufferView(MemoryBufferView &&o) noexcept : Data(o.Data), Size(o.Size) {
    o.Data = nullptr;
    o.Size = 0;
}

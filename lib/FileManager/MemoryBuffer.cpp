#include "scc/FileManager/MemoryBufferView.h"
#include <ostream>

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

std::string_view MemoryBufferView::getStringView(const MemoryViewPos &Begin,
                                                 const MemoryViewPos &End) const {
    return std::string_view(Data + Begin.Buff, End.Buff - Begin.Buff);
}

void PosView::print(std::ostream &OS) const {
    OS << (FID ? FID->getBaseName() : "<stdin>") << ':' << P.Line << ':' << P.Column;
}

std::ostream &scc::operator<<(std::ostream &OS, const PosView &V) {
    V.print(OS);
    return OS;
}

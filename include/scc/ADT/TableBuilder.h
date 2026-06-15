#ifndef SCC_ADT_TABLEBUILDER_H
#define SCC_ADT_TABLEBUILDER_H

#include <array>
#include <cstddef>

namespace scc {

template <typename Key, typename T, size_t N>
struct TableBuilder {
    std::array<T, N> Data{};

    constexpr TableBuilder &add(Key Index, T Value) {
        Data[static_cast<size_t>(Index)] = Value;
        return *this;
    }

    constexpr std::array<T, N> build() { return Data; }
};

} // namespace scc

#endif // SCC_ADT_TABLEBUILDER_H

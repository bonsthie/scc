#ifndef SCC_ADT_TABLEBUILDER_H
#define SCC_ADT_TABLEBUILDER_H

#include <array>
#include <cstddef>

namespace scc {

template <typename Key, typename T, size_t N>
struct TableBuilder {
    std::array<T, N> data{};

    constexpr TableBuilder &add(Key key, T value) {
        data[static_cast<size_t>(key)] = value;
        return *this;
    }

    constexpr std::array<T, N> build() { return data; }
};

} // namespace scc

#endif // SCC_ADT_TABLEBUILDER_H

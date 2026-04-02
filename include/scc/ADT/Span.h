#ifndef SCC_ADT_SPAN_H
#define SCC_ADT_SPAN_H

#include <cstddef>
#include <iterator>

namespace scc {

template <typename T, size_t BaseCount = static_cast<size_t>(-1)>
class Span {
  protected:
    T     *Data = nullptr;
    size_t Count = BaseCount;

    void setData(T *Ptr) { Data = Ptr; }
    void setSize(size_t N) { Count = N; }

  public:
    using iterator = T *;
    using const_iterator = const T *;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    Span() = default;
    Span(T *Ptr, size_t Size = BaseCount) : Data(Ptr), Count(Size) {}

    iterator       begin() { return Data; }
    iterator       end() { return Data + Count; }
    const_iterator begin() const { return Data; }
    const_iterator end() const { return Data + Count; }

    reverse_iterator       rbegin() { return reverse_iterator(end()); }
    reverse_iterator       rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    bool   empty() const { return Count == 0; }
    size_t size() const { return Count; }

    T       *data() { return Data; }
    const T *data() const { return Data; }

    T       &operator[](size_t i) { return Data[i]; }
    const T &operator[](size_t i) const { return Data[i]; }
};

} // namespace scc

#endif // SCC_ADT_SPAN_H

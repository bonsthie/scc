#ifndef SCC_ADT_SPAN_H
#define SCC_ADT_SPAN_H

#include <cstddef>

namespace scc {

template <typename T, size_t BaseCount = -1>
class Span {
  protected:
    T     *Data = nullptr;
    size_t Count = BaseCount;

    void setData(T *Ptr) { Data = Ptr; }
    void setSize(size_t N) { Count = N; }

  public:
    using iterator = T *;
    using const_iterator = const T *;

    Span() = default;
    Span(T *Ptr, size_t Size = BaseCount) : Data(Ptr), Count(Size) {}

    iterator       begin() { return Data; }
    iterator       end() { return Data + Count; }
    const_iterator begin() const { return Data; }
    const_iterator end() const { return Data + Count; }

    bool   empty() const { return Count == 0; }
    size_t size() const { return Count; }

    T       *data() { return Data; }
    const T *data() const { return Data; }

    T       &operator[](size_t i) { return Data[i]; }
    const T &operator[](size_t i) const { return Data[i]; }
};

} // namespace scc

#endif // SCC_ADT_SPAN_H

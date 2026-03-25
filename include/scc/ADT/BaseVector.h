#ifndef SCC_ADT_BASEVECTOR_H
#define SCC_ADT_BASEVECTOR_H

#include <concepts>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <utility>

#include "scc/ADT/Span.h"

namespace scc {

template <typename A>
concept ByteAllocator = requires(A a, void *ptr, size_t size, size_t align) {
    { a.allocate_bytes(size, align) } -> std::same_as<void *>;
    { a.deallocate_bytes(ptr, size, align) } -> std::same_as<void>;
};

template <typename T, size_t BaseSize, ByteAllocator Alloc>
class BaseVector : public Span<T, BaseSize>, protected Alloc {
    using Base = Span<T, BaseSize>;

    alignas(T) std::byte StackBuffer[BaseSize * sizeof(T)];
    size_t Capacity = BaseSize;
    bool   IsHeapAllocated = false;

  public:
    BaseVector() { initializeStorage(); }

    template <typename... AllocArgs>
        requires(sizeof...(AllocArgs) > 0 && std::constructible_from<Alloc, AllocArgs...>)
    explicit BaseVector(AllocArgs &&...allocArgs)
        : Alloc(std::forward<AllocArgs>(allocArgs)...) {
        initializeStorage();
    }
    ~BaseVector() {
        destroyElements();
        releaseHeapStorage();
    }

    BaseVector(const BaseVector &) = delete;
    BaseVector &operator=(const BaseVector &) = delete;

    size_t capacity() const { return Capacity; }

    void push_back(const T &Value) {
        ensureCapacity();
        new (&Base::data()[Base::size()]) T(Value);
        Base::setSize(Base::size() + 1);
    }

    void push_back(T &&Value) {
        ensureCapacity();
        new (&Base::data()[Base::size()]) T(std::move(Value));
        Base::setSize(Base::size() + 1);
    }

    void pop_back() {
        if (Base::size() == 0)
            return;
        size_t NewSize = Base::size() - 1;
        Base::data()[NewSize].~T();
        Base::setSize(NewSize);
    }

    void clear() {
        destroyElements();
        Base::setSize(0);
    }

  private:
    void initializeStorage() {
        Base::setData(reinterpret_cast<T *>(StackBuffer));
        Base::setSize(0);
    }

    void ensureCapacity() {
        if (Base::size() < Capacity)
            return;

        size_t NewCapacity = Capacity ? Capacity * 2 : (BaseSize ? BaseSize : size_t(1));
        T     *NewData = static_cast<T *>(allocator().allocate_bytes(NewCapacity * sizeof(T), alignof(T)));

        relocateElements(NewData);

        if (IsHeapAllocated)
            allocator().deallocate_bytes(Base::data(), Capacity * sizeof(T), alignof(T));

        Base::setData(NewData);
        Capacity = NewCapacity;
        IsHeapAllocated = true;
    }

    void destroyElements() {
        for (size_t i = 0; i < Base::size(); ++i)
            Base::data()[i].~T();
        Base::setSize(0);
    }

    void releaseHeapStorage() {
        if (IsHeapAllocated)
            allocator().deallocate_bytes(Base::data(), Capacity * sizeof(T), alignof(T));

        Base::setData(reinterpret_cast<T *>(StackBuffer));
        Capacity = BaseSize;
        IsHeapAllocated = false;
    }

    void relocateElements(T *NewData) {
        if constexpr (std::is_trivially_copyable_v<T>) {
            std::memcpy(NewData, Base::data(), Base::size() * sizeof(T));
        } else {
            for (size_t i = 0; i < Base::size(); ++i) {
                new (&NewData[i]) T(std::move(Base::data()[i]));
                Base::data()[i].~T();
            }
        }
    }

    Alloc       &allocator() { return static_cast<Alloc &>(*this); }
    const Alloc &allocator() const { return static_cast<const Alloc &>(*this); }
};

} // namespace scc

#endif // SCC_ADT_BASEVECTOR_H

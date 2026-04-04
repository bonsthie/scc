#ifndef SCC_ADT_BASEVECTOR_H
#define SCC_ADT_BASEVECTOR_H

#include <concepts>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <iterator>
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

    size_t Capacity = 0;

  public:
    BaseVector() { initializeStorage(); }

    template <typename... AllocArgs>
        requires(sizeof...(AllocArgs) > 0 && std::constructible_from<Alloc, AllocArgs...> &&
                 (!std::input_iterator<std::decay_t<AllocArgs>> && ...))
    explicit BaseVector(AllocArgs &&...allocArgs)
        : Alloc(std::forward<AllocArgs>(allocArgs)...) {
        initializeStorage();
    }

    template <std::input_iterator It, std::sentinel_for<It> Sent>
    BaseVector(It first, Sent last) {
        initializeStorage();
        appendRange(first, last);
    }

    BaseVector(std::initializer_list<T> init) {
        initializeStorage();
        appendRange(init.begin(), init.end());
    }

    ~BaseVector() {
        destroyElements();
        releaseHeapStorage();
    }

    BaseVector(const BaseVector &Other)
        requires(std::copy_constructible<Alloc> && std::copy_constructible<T>)
        : Base(),
          Alloc(static_cast<const Alloc &>(Other)) {
        initializeStorage();
        copyFrom(Other);
    }

    BaseVector(BaseVector &&Other) noexcept(std::is_nothrow_move_constructible_v<Alloc>)
        requires std::movable<Alloc>
        : Base(),
          Alloc(std::move(static_cast<Alloc &>(Other))) {
        initializeStorage();
        moveFrom(std::move(Other));
    }

    BaseVector &operator=(const BaseVector &Other)
        requires(std::is_copy_assignable_v<Alloc> && std::copy_constructible<Alloc> &&
                 std::copy_constructible<T>) {
        if (this == &Other)
            return *this;
        static_cast<Alloc &>(*this) = static_cast<const Alloc &>(Other);
        copyFrom(Other);
        return *this;
    }

    BaseVector &operator=(BaseVector &&Other) noexcept(std::is_nothrow_move_assignable_v<Alloc>)
        requires std::movable<Alloc> {
        if (this == &Other)
            return *this;
        static_cast<Alloc &>(*this) = std::move(static_cast<Alloc &>(Other));
        moveFrom(std::move(Other));
        return *this;
    }

    size_t capacity() const { return Capacity; }

    void push_back(const T &Value) { emplace_back(Value); }

    void push_back(T &&Value) { emplace_back(std::move(Value)); }

    template <typename... Args>
    T &emplace_back(Args &&...args) {
        ensureCapacityFor(Base::size() + 1);
        new (&Base::data()[Base::size()]) T(std::forward<Args>(args)...);
        Base::setSize(Base::size() + 1);
        return Base::data()[Base::size() - 1];
    }

    void pop_back() {
        if (Base::size() == 0)
            return;
        size_t NewSize = Base::size() - 1;
        Base::data()[NewSize].~T();
        Base::setSize(NewSize);
    }

    void clear() { destroyElements(); }

    bool operator==(const BaseVector &Other) const
        requires requires(const T &a, const T &b) { { a == b } -> std::convertible_to<bool>; }
    {
        if (Base::size() != Other.size())
            return false;
        for (size_t i = 0; i < Base::size(); ++i) {
            if (!(Base::data()[i] == Other.data()[i]))
                return false;
        }
        return true;
    }

    bool operator!=(const BaseVector &Other) const
        requires requires(const T &a, const T &b) { { a == b } -> std::convertible_to<bool>; }
    {
        return !(*this == Other);
    }

    T       &front() { return Base::data()[0]; }
    const T &front() const { return Base::data()[0]; }

    T       &back() { return Base::data()[Base::size() - 1]; }
    const T &back() const { return Base::data()[Base::size() - 1]; }

    void reserve(size_t NewCapacity) { ensureCapacityFor(NewCapacity); }

    typename Base::iterator erase(typename Base::iterator pos) {
        if (pos == Base::end())
            return pos;

        auto start = Base::data();
        auto index = static_cast<size_t>(pos - start);
        auto count = Base::size();

        if constexpr (std::is_trivially_copyable_v<T>) {
            std::memmove(start + index, start + index + 1, (count - index - 1) * sizeof(T));
        } else {
            for (size_t i = index; i + 1 < count; ++i) {
                start[i].~T();
                new (&start[i]) T(std::move(start[i + 1]));
            }
            start[count - 1].~T();
        }

        Base::setSize(count - 1);
        return start + index;
    }

  private:
    void initializeStorage() {
        Base::setData(nullptr);
        Base::setSize(0);
        Capacity = 0;
    }

    void ensureCapacityFor(size_t Required) {
        if (Capacity >= Required)
            return;

        size_t NewCapacity = Capacity ? Capacity : (BaseSize ? BaseSize : size_t(1));
        if (NewCapacity == 0)
            NewCapacity = 1;
        while (NewCapacity < Required)
            NewCapacity *= 2;
        reallocate(NewCapacity);
    }

    void reallocate(size_t NewCapacity) {
        T *NewData = static_cast<T *>(allocator().allocate_bytes(NewCapacity * sizeof(T), alignof(T)));
        relocateElements(NewData);

        releaseStorage();
        Base::setData(NewData);
        Capacity = NewCapacity;
    }

    void destroyElements() {
        for (size_t i = 0; i < Base::size(); ++i)
            Base::data()[i].~T();
        Base::setSize(0);
    }

    void releaseHeapStorage() { releaseStorage(); }

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

    void copyFrom(const BaseVector &Other) {
        destroyElements();
        ensureCapacityFor(Other.size());
        for (size_t i = 0; i < Other.size(); ++i)
            new (&Base::data()[i]) T(Other.data()[i]);
        Base::setSize(Other.size());
    }

    void moveFrom(BaseVector &&Other) {
        destroyElements();
        releaseStorage();

        Base::setData(Other.data());
        Capacity = Other.Capacity;
        Base::setSize(Other.size());

        Other.Base::setData(nullptr);
        Other.Capacity = 0;
        Other.Base::setSize(0);
    }

    void appendRange(auto first, auto last) {
        for (; first != last; ++first)
            emplace_back(*first);
    }

    void releaseStorage() {
        if (Base::data()) {
            allocator().deallocate_bytes(Base::data(), Capacity * sizeof(T), alignof(T));
            Base::setData(nullptr);
            Capacity = 0;
        }
    }

    Alloc       &allocator() { return static_cast<Alloc &>(*this); }
    const Alloc &allocator() const { return static_cast<const Alloc &>(*this); }
};

} // namespace scc

#endif // SCC_ADT_BASEVECTOR_H

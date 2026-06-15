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
concept ByteAllocator = requires(A a, void *Ptr, size_t Size, size_t Align) {
    { a.allocateBytes(Size, Align) } -> std::same_as<void *>;
    { a.deallocateBytes(Ptr, Size, Align) } -> std::same_as<void>;
};

template <typename T, size_t BaseSize, ByteAllocator Alloc>
// NOLINTNEXTLINE(misc-multiple-inheritance)
class BaseVector : public Span<T, BaseSize>, protected Alloc {
    using Base = Span<T, BaseSize>;

    size_t Capacity = 0;

  public:
    BaseVector() { initializeStorage(); }

    template <typename... AllocArgs>
        requires(sizeof...(AllocArgs) > 0 && std::constructible_from<Alloc, AllocArgs...> &&
                 (!std::input_iterator<std::decay_t<AllocArgs>> && ...))
    explicit BaseVector(AllocArgs &&...VAllocArgs)
        : Alloc(std::forward<AllocArgs>(VAllocArgs)...) {
        initializeStorage();
    }

    template <std::input_iterator It, std::sentinel_for<It> Sent>
    BaseVector(It First, Sent Last) {
        initializeStorage();
        appendRange(First, Last);
    }

    BaseVector(std::initializer_list<T> Init) {
        initializeStorage();
        appendRange(Init.begin(), Init.end());
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

    void pushBack(const T &Value) { emplaceBack(Value); }

    void pushBack(T &&Value) { emplaceBack(std::move(Value)); }

    template <typename... Args>
    T &emplaceBack(Args &&...VArgs) {
        ensureCapacityFor(Base::size() + 1);
        new (&Base::data()[Base::size()]) T(std::forward<Args>(VArgs)...);
        Base::setSize(Base::size() + 1);
        return Base::data()[Base::size() - 1];
    }

    void popBack() {
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

    typename Base::iterator erase(typename Base::iterator Pos) {
        if (Pos == Base::end())
            return Pos;

        auto Start = Base::data();
        auto Index = static_cast<size_t>(Pos - Start);
        auto Count = Base::size();

        if constexpr (std::is_trivially_copyable_v<T>) {
            std::memmove(Start + Index, Start + Index + 1, (Count - Index - 1) * sizeof(T));
        } else {
            for (size_t i = Index; i + 1 < Count; ++i) {
                Start[i].~T();
                new (&Start[i]) T(std::move(Start[i + 1]));
            }
            Start[Count - 1].~T();
        }

        Base::setSize(Count - 1);
        return Start + Index;
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
        T *NewData = static_cast<T *>(allocator().allocateBytes(NewCapacity * sizeof(T), alignof(T)));
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

    void appendRange(auto First, auto Last) {
        for (; First != Last; ++First)
            emplaceBack(*First);
    }

    void releaseStorage() {
        if (Base::data()) {
            allocator().deallocateBytes(Base::data(), Capacity * sizeof(T), alignof(T));
            Base::setData(nullptr);
            Capacity = 0;
        }
    }

    Alloc       &allocator() { return static_cast<Alloc &>(*this); }
    const Alloc &allocator() const { return static_cast<const Alloc &>(*this); }
};

} // namespace scc

#endif // SCC_ADT_BASEVECTOR_H

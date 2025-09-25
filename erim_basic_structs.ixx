/// @copyright (c) 2025, Erivaldo Jair Xavier Mate
/** @brief

    This module defines fundamental data structures and type utilities for the Erim Modules.
    It provides generic non STL, array types, iterators, join/merge/sort helpers, and variable-length
    containers. These building blocks are used throughout the codebase to ensure consistency
    and reusability for basic data handling, independent of threading or concurrency.

    Key features:
      - Comparable Iterator
      - Static and Variable length arrays
**/
module;
#include <new>
#include <stdlib.h>
export module erim_basic_structs;
import erim_primitives;
import erim_match_any;
import erim_conditional;

namespace erim
{
    //empty class designed to be used as base
    template <typename CRTP = void>
    struct Iterative;

    export {
        template <typename T, auto... singularities>
        struct Container;

        ///@b Array, Sequence of values with same Type, contains[T data[N]]
        template <typename T, size_t N>
        struct Array;

        ///@b Varlena, Variable length Array, contains[size_t count_v, T data [N]]
        template <typename T, size_t N = 0>
        struct Varlena;

        ///@b Vector, Pointer to varlena with RAII , contains[Varlena* ptr]
        template <typename T>
        struct Vector;

        template <typename T, size_t N, bool enabled_if = true>
        requires(enabled_if)
        using array_t = T[N];

        template <typename T, bool enabled_if = true>
        requires(enabled_if)
        using pointer_t = T*;
    }
};

template <typename CRTP>
concept AnyIterable = requires(const CRTP& v) {
    static_cast<const erim::Iterative<CRTP>&>(v);
    v.begin();
    v.end();
    v.count();
};

template <>
struct erim::Iterative<void> {
   private:
    static constexpr void
    real_sort(const auto iterator, const size_t size, const auto aux) noexcept {
        for (size_t width = 1; width < size; width *= 2) {
            for (size_t i = 0; i < size; i += 2 * width) {
                size_t left  = i;
                size_t mid   = i + width < size ? i + width : size;
                size_t right = i + 2 * width < size ? i + 2 * width : size;
                size_t l = left, r = mid, t = 0;
                while (l < mid && r < right) {
                    if (iterator[l] < iterator[r])
                        aux[t++] = iterator[l++];
                    else
                        aux[t++] = iterator[r++];
                }
                while (l < mid) aux[t++] = iterator[l++];
                while (r < right) aux[t++] = iterator[r++];
                for (size_t k = 0; k < t; ++k) iterator[left + k] = aux[k];
            }
        }
    }
   protected:
    template < AnyIterable T, typename U>
    static inline constexpr void sort(T* it, U aux) noexcept {
        auto& indirection = real_sort<decltype(it->begin()), U>;
        return indirection(it->begin(), it->count(), aux);
    }

    template <AnyIterable T, AnyIterable U>
    friend inline constexpr bool operator==(T lhs, U rhs) noexcept {
        if (lhs.count() != rhs.count()) return false;
        size_t i = 0;
        while (i != lhs.count() && lhs[i] == rhs[i]) ++i;
        return i == lhs.count();
    }
    template <AnyIterable T, AnyIterable U>
    friend inline constexpr bool operator!=(T lhs, U rhs) noexcept {
        return !(lhs == rhs);
    }
    template <AnyIterable T, AnyIterable U>
    friend inline constexpr bool operator<(T lhs, U rhs) noexcept {
        size_t min = (lhs.count() < rhs.count()) ? lhs.count() : rhs.count();
        size_t i   = 0;
        while (i != min && lhs[i] == rhs[i]) ++i;
        if (i != min) return lhs[i] < rhs[i];
        return lhs.count() < rhs.count();
    }
    template <AnyIterable T, AnyIterable U>
    friend inline constexpr bool operator>(T lhs, U rhs) noexcept {
        return rhs < lhs;
    }
    template <AnyIterable T, AnyIterable U>
    friend inline constexpr bool operator<=(T lhs, U rhs) noexcept {
        return !(rhs < lhs);
    }
    template <AnyIterable T, AnyIterable U>
    friend inline constexpr bool operator>=(T lhs, U rhs) noexcept {
        return !(lhs < rhs);
    }
};
template <typename CRTP>
struct erim::Iterative : erim::Iterative<void> {
    /// **** @brief CRTP redundant methods ***
    /// @b ((CRTP&)*this).begin()   @c always_required
    /// @b ((CRTP&)*this)[<size_t>] @c always_optional
    /// @b ((CRTP&)*this).end()     @c optional_when_count()_method_is_defined
    /// @b ((CRTP&)*this).count()   @c optional_when_end()_method_is_defined

    inline constexpr decltype(auto) end() const noexcept {
        return ((CRTP&)*this).begin() + ((CRTP&)*this).count();
    }

    inline constexpr decltype(auto) end() noexcept {
        return ((CRTP&)*this).begin() + ((CRTP&)*this).count();
    }

    inline constexpr decltype(auto) count() const noexcept {
        return ((CRTP&)*this).end() - ((CRTP&)*this).begin();
    }

    inline constexpr decltype(auto) count() noexcept {
        return ((CRTP&)*this).end() - ((CRTP&)*this).begin();
    }

    inline constexpr decltype(auto) operator[](size_t i) const noexcept {
        return ((CRTP&)*this).begin()[i];
    }

    inline constexpr decltype(auto) operator[](size_t i) noexcept {
        return ((CRTP&)*this).begin()[i];
    }

    //sort
    template <typename T>
    inline constexpr void sort_with_aux(T aux) noexcept {
        erim::Iterative<void>::sort(((CRTP*)this), aux);
    }

    template <typename Trivial>
    inline constexpr void sort_with_aux() noexcept {
        Trivial aux;
        erim::Iterative<void>::sort(((CRTP*)this), aux);
    }
    inline bool sort() noexcept {
        auto* aux = new (std::nothrow) CRTP::element_t[((CRTP&)*this).count()];
        if (!aux) return 0;
        sort_with_aux(aux);
        delete[] aux;
        return 1;
    }

    // Get Max
    constexpr inline decltype(auto) get_max() noexcept {
        auto& result = ((CRTP&)*this).begin()[0];
        for (auto& val : (CRTP&)*this)
            if (val > result) result = val;
        return result;
    }

    constexpr inline decltype(auto) get_max() const noexcept {
        auto& result = ((CRTP&)*this).begin()[0];
        for (auto& val : (CRTP&)*this)
            if (val > result) result = val;
        return result;
    }

    // Get Min
    constexpr inline decltype(auto) get_min() noexcept {
        auto& result = ((CRTP&)*this).begin()[0];
        for (auto& val : (CRTP&)*this)
            if (val < result) result = val;
        return result;
    }

    constexpr inline decltype(auto) get_min() const noexcept {
        auto& result = ((CRTP&)*this).begin()[0];
        for (auto& val : (CRTP&)*this)
            if (val < result) result = val;
        return result;
    }
};

template <auto... singularities>
struct erim::Container<void, singularities...> {};

template <typename T, auto... singularities>
struct erim::Container {
    union {
        uint8_t erasable[sizeof(T)];
        T data;
    };

   protected:
    inline constexpr void
    erase_bytes(const size_t N, const uint8_t v = 0) noexcept {
        for (size_t i = 0; i < N; ++i) erasable[i] = v;
    };
   public:
    template <typename... U>
    inline constexpr Container(U... v) noexcept : data{((U)v)...} {}
    inline constexpr Container() noexcept {}
    inline constexpr ~Container() noexcept {}

    inline constexpr operator const T&() const noexcept { return data; }
    inline constexpr operator T&() noexcept { return data; }

    inline constexpr decltype(auto) operator->() const noexcept requires(
        not requires { data.operator->(); } && requires { &*data; }
    )
    {
        return &*data;
    }
    inline constexpr decltype(auto) operator->() noexcept requires(
        not requires { data.operator->(); } && requires { &*data; }
    )
    {
        return &*data;
    }

    inline constexpr decltype(auto)
    operator->() const noexcept requires requires { data.operator->(); }
    {
        return data.operator->();
    }
    inline constexpr decltype(auto)
    operator->() noexcept requires requires { data.operator->(); }
    {
        return data.operator->();
    }
};

template <typename T, size_t N>
struct erim::Array : Iterative<Array<T, N>>, Container<T[N]> {
    using element_t = T;
    using arr_t     = array_t<T, N, N != 0>;

    inline constexpr ~Array() noexcept {
        for (auto& v : *this) v.~T();
    }
    inline constexpr Array() { this->erase_bytes(sizeof(T) * N); };
    inline constexpr Array(const arr_t& pp) noexcept : Array{} {
        for (size_t i = 0; i < N; ++i) this->data[i] = (T&)pp[i];
    }
    inline constexpr Array(arr_t&& pp) noexcept : Array{} {
        for (size_t i = 0; i < N; ++i) this->data[i] = (T&&)pp[i];
    }
    inline constexpr size_t count() const noexcept { return N; }
    inline constexpr const T* begin() const noexcept { return this->data; }
    inline constexpr T* begin() noexcept { return this->data; }

    inline constexpr void sort() noexcept requires(sizeof(arr_t) <= (1 << 20))
    {
        this->template sort_with_aux<Array<T, N>>();
    }
};

struct VarlenaHead {
    size_t count_v;
    size_t max_v;
};

template <typename T, size_t N>
struct erim::Varlena : Iterative<Varlena<T, N>>,
                       conditioned_t<N == 0, VarlenaHead, Varlena<T, 0>>,
                       private Container<conditioned_t<N == 0, void, T[N]>> {
    using Iterative<Varlena<T, N>>::end;
    using Iterative<Varlena<T, N>>::sort_with_aux;
    using Iterative<Varlena<T, N>>::sort;
    using Iterative<Varlena<T, N>>::operator[];
    using element_t = T;
    using arr_t     = array_t<T, N>;

    inline constexpr operator Varlena<T, 0>&() noexcept requires(N != 0)
    {
        return static_cast<Varlena<T, 0>&>(static_cast<VarlenaHead&>(*this));
    }

    inline constexpr
    operator const Varlena<T, 0>&() const noexcept requires(N != 0)
    {
        return static_cast<const Varlena<T, 0>&>(
            static_cast<const VarlenaHead&>(*this)
        );
    }

    inline constexpr ~Varlena() noexcept {
        if constexpr (N != 0)
            for (auto& v : *this) v.~T();
    }
    inline constexpr Varlena() : Varlena<T, 0>{N, N} {
        if constexpr (N != 0) this->erase_bytes(sizeof(T) * N);
    };

    inline constexpr Varlena(const arr_t& pp) noexcept : Varlena{} {
        for (size_t i = 0; i < N; ++i) this->data[i] = (T&)pp[i];
    }
    inline constexpr Varlena(const arr_t&& pp) noexcept : Varlena{} {
        for (size_t i = 0; i < N; ++i) this->data[i] = (T&&)pp[i];
    }

    inline constexpr T* begin() noexcept {
        if constexpr (N == 0)
            return (T*)(&this->count_v + sz_offset);
        else
            return this->data;
    }

    inline constexpr const T* begin() const noexcept {
        if constexpr (N == 0)
            return (const T*)(&this->count_v + sz_offset);
        else
            return this->data;
    }

    inline constexpr size_t count() const noexcept { return this->count_v; }
    inline constexpr void sort() noexcept requires(sizeof(arr_t) <= (1 << 20))
    {
        this->template sort_with_aux<Array<T, N>>();
    }
    static constexpr size_t sz_offset = [] {
        return Array{{alignof(T) / sizeof(size_t), size_t(2)}}.get_max();
    }();
   private:
    inline constexpr Varlena(size_t a, size_t b) noexcept requires(N == 0)
        : VarlenaHead{a, b} {}
    template <typename T2, size_t N2>
    friend struct Varlena;
};

template <typename T>
struct erim::Vector : Iterative<Vector<T>>, Container<Varlena<T>*> {
    static constexpr size_t b_offset = Varlena<T>::sz_offset * sizeof(size_t);
    using varlena_ptr_t              = Varlena<T>*;
    inline constexpr Varlena<T>*& get_ptr() const noexcept {
        return this->data;
    }
    inline constexpr T* begin() const noexcept { return get_ptr()->begin(); }
    inline constexpr size_t count() const noexcept {
        return get_ptr() ? get_ptr()->count() : 0;
    }

    inline constexpr ~Vector() noexcept {
        for (auto& v : varlena()) v.~T();
        delete this->data;
    }

    template <size_t N>
    inline constexpr Vector(Varlena<T, N>*&& varlena = 0) noexcept
        : Container<Varlena<T>*>{varlena} {
        varlena = 0;
    }

    inline constexpr Vector(size_t N) noexcept {
        get_ptr() = malloc(b_offset + N * sizeof(T));
        if (get_ptr()) *get_ptr() = VarlenaHead{0, N};
    }
};

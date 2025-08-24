module;
#include <cstdint>
#include <atomic>
export module erim_primitives;

#define MAX(A, B) ((A) > (B) ? (A) : (B))
export {
    /// @b atomic is std::atomic_ref. that's the only true way of using atomics,
    /// an object is not atomic, atomic are the operations on memory.
    template <typename T>
    using atomic       = std::atomic_ref<T>;
    using atomic_order = std::memory_order;
    using std::atomic_signal_fence;  // NOLINT(misc-unused-using-decls)
    using std::atomic_thread_fence;  // NOLINT(misc-unused-using-decls)

    // clang-format off
    template <typename T> struct Half     { static constexpr bool exists = 0; };
	template <typename T> struct Signed   { static constexpr bool exists = 0; };
	template <typename T> struct Unsigned { static constexpr bool exists = 0; };

    template <typename T> using half_t = Half<T>::type;
	template <typename T> using signed_t = Signed<T>::type;
	template <typename T> using unsigned_t = Unsigned<T>::type;

    template <class T>constexpr bool is_signed_v = Unsigned<T>::exists;
	template <class T>constexpr bool is_unsigned_v = Signed<T>::exists;

    using std::int16_t;
    using std::int32_t;
    using std::int64_t;
    using std::int8_t;

    using std::int_fast16_t;
    using std::int_fast32_t;
    using std::int_fast64_t;
    using std::int_fast8_t;

    using std::int_least16_t;
    using std::int_least32_t;
    using std::int_least64_t;
    using std::int_least8_t;

    using std::intmax_t;
    using std::intptr_t;

    using std::uint16_t;
    using std::uint32_t;
    using std::uint64_t;
    using std::uint8_t;

    using std::uint_fast16_t;
    using std::uint_fast32_t;
    using std::uint_fast64_t;
    using std::uint_fast8_t;

    using std::uint_least16_t;
    using std::uint_least32_t;
    using std::uint_least64_t;
    using std::uint_least8_t;

    using std::uintmax_t;
    using std::uintptr_t;

	template <> struct Half<uint16_t>    { static constexpr bool exists = 1; using type = uint8_t;  };
	template <> struct Half<uint32_t>    { static constexpr bool exists = 1; using type = int16_t;  };
	template <> struct Half<uint64_t>    { static constexpr bool exists = 1; using type = int32_t;  };
	template <> struct Half<int16_t>     { static constexpr bool exists = 1; using type = int8_t;   };
	template <> struct Half<int32_t>     { static constexpr bool exists = 1; using type = int16_t;  };
	template <> struct Half<int64_t>     { static constexpr bool exists = 1; using type = int32_t;  };
	
	template <> struct Signed<uint8_t>   { static constexpr bool exists = 1; using type = int8_t;   };
	template <> struct Signed<uint16_t>  { static constexpr bool exists = 1; using type = int16_t;  };
	template <> struct Signed<uint32_t>  { static constexpr bool exists = 1; using type = int32_t;  };
	template <> struct Signed<uint64_t>  { static constexpr bool exists = 1; using type = int64_t;  };
	
	template <> struct Unsigned<int8_t>  { static constexpr bool exists = 1; using type = uint8_t;  };
	template <> struct Unsigned<int16_t> { static constexpr bool exists = 1; using type = uint16_t; };
	template <> struct Unsigned<int32_t> { static constexpr bool exists = 1; using type = uint32_t; };
	template <> struct Unsigned<int64_t> { static constexpr bool exists = 1; using type = uint64_t; };
  
#if defined(__SIZEOF_INT128__)
    using int128_t  = __int128_t;
    using uint128_t = __uint128_t;

    template <> struct Half<int128_t>    { static constexpr bool exists = 1; using type = int64_t;  };
    template <> struct Half<uint128_t>   { static constexpr bool exists = 1; using type = uint64_t; };

    template <> struct Signed<uint128_t> { static constexpr bool exists = 1; using type = int128_t; };
    template <> struct Unsigned<int128_t>{ static constexpr bool exists = 1; using type = uint128_t; };
    // clang-format on

    using size_t  = decltype(sizeof(void *));
    using ssize_t = signed_t<size_t>;
#endif

    template <unsigned ALIGNAMENT, typename T = char[ALIGNAMENT]>
    struct alignas(ALIGNAMENT) aligned_t;

    template <unsigned ALIGNAMENT, typename T>
    requires(ALIGNAMENT >= 16)
    struct alignas(ALIGNAMENT) aligned_t<ALIGNAMENT, T> {
        static constexpr unsigned BITS =  8 * MAX(sizeof(T), ALIGNAMENT);
        union {
#if defined(__SIZEOF_INT128__)
            int128_t i128[BITS / 128];
            uint128_t u128[BITS / 128];
#endif
            int64_t i64[BITS / 64];
            int32_t i32[BITS / 32];
            int16_t i16[BITS / 16];
            int8_t i8[BITS / 8];
            uint64_t u64[BITS / 64];
            uint32_t u32[BITS / 32];
            uint16_t u16[BITS / 16];
            uint8_t u8[BITS / 8];
            char str_arr[BITS / 8];
            void *data_ptr;
            char *str_ptr;
            T value;
        };
    };

    template <unsigned ALIGNAMENT, typename T>
    requires(ALIGNAMENT == 8)
    struct alignas(ALIGNAMENT) aligned_t<ALIGNAMENT, T> {
        static constexpr unsigned BITS = 8 * MAX(sizeof(T), ALIGNAMENT);
        union {
            int64_t i64[BITS / 64];
            int32_t i32[BITS / 32];
            int16_t i16[BITS / 16];
            int8_t i8[BITS / 8];
            uint64_t u64[BITS / 64];
            uint32_t u32[BITS / 32];
            uint16_t u16[BITS / 16];
            uint8_t u8[BITS / 8];
            char str_arr[BITS / 8];
            void *data_ptr;
            char *str_ptr;
            T value;
        };
    };

    template <unsigned ALIGNAMENT, typename T>
    requires(ALIGNAMENT == 4)
    struct alignas(ALIGNAMENT) aligned_t<ALIGNAMENT, T> {
        static constexpr unsigned BITS = 8 * MAX(sizeof(T), ALIGNAMENT);
        union {
            int32_t i32[BITS / 32];
            int16_t i16[BITS / 16];
            int8_t i8[BITS / 8];
            uint32_t u32[BITS / 32];
            uint16_t u16[BITS / 16];
            uint8_t u8[BITS / 8];
            char str_arr[BITS / 8];
            T value;
        };
    };

    template <unsigned ALIGNAMENT, typename T>
    requires(ALIGNAMENT == 2)
    struct alignas(ALIGNAMENT) aligned_t<ALIGNAMENT, T> {
        static constexpr unsigned BITS = 8 * MAX(sizeof(T), ALIGNAMENT);
        union {
            int16_t i16[BITS / 16];
            int8_t i8[BITS / 8];
            uint16_t u16[BITS / 16];
            uint8_t u8[BITS / 8];
            char str_arr[BITS / 8];

            T value;
        };
    };

    template <unsigned ALIGNAMENT, typename T>
    requires(ALIGNAMENT == 1)
    struct alignas(ALIGNAMENT) aligned_t<ALIGNAMENT, T> {
        static constexpr unsigned BITS = 8 * MAX(sizeof(T), ALIGNAMENT);
        union {
            int16_t i16[BITS / 16];
            int8_t i8[BITS / 8];
            uint16_t u16[BITS / 16];
            uint8_t u8[BITS / 8];
            char str_arr[BITS / 8];
            T value;
        };
    };
}

export module erim_match_any;

#define ERIM_MATCH_TEMPLATE(GENERIC_NAME, TPARAM_FORMAT)                      \
    namespace erim::Helper                                                    \
    {                                                                         \
        template <TPARAM_FORMAT A, TPARAM_FORMAT... B>                        \
        struct GENERIC_NAME {                                                 \
            static constexpr bool match_one = 0;                              \
            static constexpr int left       = -1;                             \
        };                                                                    \
                                                                              \
        template <TPARAM_FORMAT A, TPARAM_FORMAT... B>                        \
        struct GENERIC_NAME<A, A, B...> {                                     \
            static constexpr bool match_one = 1;                              \
            static constexpr int left       = sizeof...(B);                   \
        };                                                                    \
        template <TPARAM_FORMAT A, TPARAM_FORMAT B, TPARAM_FORMAT... C>       \
        struct GENERIC_NAME<A, B, C...> : GENERIC_NAME<A, C...> {};           \
    }                                                                         \
                                                                              \
    export template <TPARAM_FORMAT A, TPARAM_FORMAT... B>                     \
    struct GENERIC_NAME : erim::Helper::GENERIC_NAME<A, B...> {               \
        using erim::Helper::GENERIC_NAME<A, B...>::left;                      \
        using erim::Helper::GENERIC_NAME<A, B...>::match_one;                 \
        static constexpr int idx = left < 0 ? left : sizeof...(B) - 1 - left; \
        static constexpr int match_idx = idx;                                 \
    };

ERIM_MATCH_TEMPLATE(TYPES, typename)
ERIM_MATCH_TEMPLATE(CONSTANTS, auto)

namespace erim
{
    template <typename T>
    struct rm_const : rm_const<const T> {};

    template <typename T>
    struct rm_const<const T> {
        using type = T;
    };

    template <typename T>
    struct add_const : add_const<const T> {};

    template <typename T>
    struct add_const<const T> {
        using type = const T;
    };

    template <typename T, typename U>
    struct copy_const {
        using type = rm_const<U>::type;
    };

    template <typename T, typename U>
    struct copy_const<const T, U> {
        using type = add_const<U>::type;
    };

    // Pointer manipulation templates
    template <typename T>
    struct rm_pointer { using type = T; };

    template <typename T>
    struct rm_pointer<T*> { using type = T; };

    template <typename T>
    struct add_pointer { using type = T*; };

    template <typename T, typename U>
    struct copy_pointer { using type = rm_pointer<U>::type; };

    template <typename T, typename U>
    struct copy_pointer<T*, U> { using type = add_pointer<U>::type; };

    // Reference manipulation templates
    template <typename T>
    struct rm_reference { using type = T; };

    template <typename T>
    struct rm_reference<T&> { using type = T; };

    template <typename T>
    struct add_reference { using type = T&; };

    template <typename T, typename U>
    struct copy_reference { using type = rm_reference<U>::type; };

    template <typename T, typename U>
    struct copy_reference<T&, U> { using type = add_reference<U>::type; };
}

#define EXPORT_TAKE_MAKE_T(NAME)                            \
    export namespace erim                                   \
    {                                                       \
        namespace MAKE                                      \
        {                                                   \
            template <typename T>                           \
            using NAME##_t = erim::add_##NAME<T>::type;     \
        }                                                   \
        namespace REMOVE                                    \
        {                                                   \
            template <typename T>                           \
            using NAME##_t = erim::rm_##NAME<T>::type;      \
        }                                                   \
        namespace COPY                                      \
        {                                                   \
            template <typename T, typename U>               \
            using NAME##_t = erim::copy_##NAME<T, U>::type; \
        }                                                   \
    }

EXPORT_TAKE_MAKE_T(const);
EXPORT_TAKE_MAKE_T(pointer);
EXPORT_TAKE_MAKE_T(reference);

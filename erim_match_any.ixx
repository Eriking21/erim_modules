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

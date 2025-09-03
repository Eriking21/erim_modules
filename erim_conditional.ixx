export module erim_conditional;

// kaizen
export {
    namespace erim
    {
        template <bool cond>
        struct conditional {
            template <typename T1, typename T2>
            using type = T1;

            template <
                template <typename... T> class W1,
                template <typename... T> class W2,
                typename... T>
            using wrap = W1<T...>;
        };

        template <>
        struct conditional<false> {
            template <typename T1, typename T2>
            using type = T2;

            template <
                template <typename... T> class W1,
                template <typename... T> class W2,
                typename... T>
            using wrap = W2<T...>;
        };

        using true_type  = conditional<true>;
        using false_type = conditional<false>;

    }
    template <typename T>
    concept is_erim_true_type =
        requires(T& val) { static_cast<erim::true_type&>(val); };

    template <bool cond, typename T, typename U>
    using conditional_t = erim::conditional<cond>::template type<T, U>;

    template <
        bool cond,
        template <typename... T> class W1,
        template <typename... T> class W2,
        typename... T>
    using conditional_wrapped_t =
        erim::conditional<cond>::template wrap<W1, W2, T...>;

    template <typename T>
    using default_t = T;
}

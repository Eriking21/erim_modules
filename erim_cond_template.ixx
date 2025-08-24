export module erim_conditional_type;

// kaizen
namespace erim::HELPER
{
    template <bool cond>
    struct conditional {
        template <typename T, typename U>
        using type = T;

        template <
            template <typename... T> class W1,
            template <typename... T> class W2,
            typename... T>
        using wrap = W1<T...>;
    };

    template <>
    struct conditional<false> {
        template <typename T, typename U>
        using type = U;

        template <
            template <typename... T> class W1,
            template <typename... T> class W2,
            typename... T>
        using wrap = W2<T...>;
    };
    using conditional_true_t  = conditional<true>;
    using conditional_false_t = conditional<false>;
}

export template <bool cond, typename T, typename U>
using conditional_t = erim::HELPER::conditional<cond>::template type<T, U>;

export template <
    bool cond,
    template <typename... T> class W1,
    template <typename... T> class W2,
    typename... T>
using conditional_wrapped_t =
    erim::HELPER::conditional<cond>::template wrap<W1, W2, T...>;

template <typename T>
using default_t = T;
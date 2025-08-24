export module erim_super_types;

export {
    ///@b Aligned_Object :  /
    template <typename T>
    class Class_Holder {
        using type = T;
    };

    template <typename T>
    struct Interval_Of {
        T *const begin_;
        T *const end_;
        constexpr T *begin() const noexcept { return begin_; }
        constexpr T *end() const noexcept { return end_; }
    };

    template < auto Deleter, typename T>
    class RAII {
        T resource;
       public:
        inline constexpr explicit RAII(T res) : resource(res) {}
        inline constexpr ~RAII() { Deleter(resource); }
        inline constexpr T get() const { return resource; }
        inline constexpr operator T() const { return resource; }
    };

    // must be last member
    template < typename T>
    struct Flexible_Array {
        T array[];
        inline constexpr operator T *() const noexcept { return array; }
    };

    template <typename T = int>
    struct Flags {
        T flag;
        inline constexpr Flags(const auto val) noexcept
            : flag(static_cast<T>(val)) {};
        inline constexpr operator T() const noexcept { return flag; }

        inline constexpr Flags with(const T flags) const noexcept {
            return Flags(flag | flags);
        }
        inline constexpr Flags without(const T flags) const noexcept {
            return Flags(flag & ~flags);
        }
        inline constexpr bool includes(const T flags) const noexcept {
            return (flag & flags) == flags;
        }
        inline constexpr bool excludes(const T flags) const noexcept {
            return (flag & flags) == 0;
        }
    };
}

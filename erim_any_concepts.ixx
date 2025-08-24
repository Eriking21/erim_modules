export module erim_any_concepts;

export {
    // clang-format off
    template <typename T> concept ANY_VOID = not requires { sizeof(T); };\

    template <typename T> concept ANY_REFERENCE = requires { sizeof(T); } && not requires(T* p) { 1; };

    template <typename T> concept ANY_STRUCT = requires(int T::* m) { 1; };

    template <typename T> concept ANY_POINTER = requires(T a) { *a, a = 0; } && not requires(int T::* m) { 1; };

    template <typename T> concept ANY_ARRAY = requires(T a) { *a; } && not requires(T a, int T::* m) { a = 0; };

    template <typename T> concept ANY_PRIMITIVE = requires(T* p) { sizeof(T); } && not requires(T a, int T::* m) { *a; };

    template <typename T> concept ANY_EXCEPTS_VOID = not ANY_VOID<T>;

    template <typename T> concept ANY_EXCEPTS_POINTER = not ANY_POINTER<T>;

    template <typename T> concept ANY_EXCEPTS_REFERENCE = not ANY_REFERENCE<T>;

    template <typename T, typename Base> concept ANY_DERIVED_FROM =  requires { static_cast<Base*>(static_cast<T*>(nullptr)); };
    // clang-format on
}
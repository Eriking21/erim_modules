/// @copyright (c) 2025, Erivaldo Jair Xavier Mate
/// @note
//  - atomic: like std::atomic_ref
//  - atomic_order: like std::memory_order
/** @brief

        This @b interface is designed to handle @b Multithreading
        using separate namespaces and a base Object class for each.
        the concept is that a concurrent object, is a single instance/resource,
        controlled by a single thread/worker, and acessible to others as pointers.

        @b Concurrent-objects cannot be constructed arbitrarily, they must be part
        of a non-concurrent @b thread-local-bject. Only the owning thread is
        expected to bypass thread-safety mechanisms; all others must use safe access.
        
        The @b Pool's purpose is to create, destroy, and delegate tasks to threads,
        @b erim::thread:::pool manages multiple threads, assigning a task to all,
        after task completion, concurrents threads will busy wait for a new task,
        the owner thread may also participate, without the busy wait [it bypasses].
            - if you need them to wait, set a task to wait or sleep;
            - if you need multitasking, set a task to fetch from a concurrent::queue.
            - if you need them to stop, set a null as task, but not as first task.

**/
export module erim_concurrent;
import erim_primitives;
import erim_basic_structs;

// clang-format off
export namespace erim {
    namespace thread { struct Object; }
    namespace concurrent { struct Object; }
}

export namespace erim::thread {
    template <typename Thread, typename Resource, auto first_task>
    struct Pool;
};

export namespace erim::concurrent {
    template <typename Resource>
    using task_t = void (*)(Resource*) noexcept;
    
    template <typename Resource, auto>
    struct Task {};
    
    template <typename Resource, auto fn>
    requires requires(task_t<Resource> task__, Resource& r) {
        static_cast<Object&>(r), task__ = Task<Resource, fn>::callback;}
    constexpr inline task_t<Resource> task_v {Task<Resource, fn>::callback};
}
// clang-format on

///@b  erim::concurrent::Object
struct alignas(64) erim::concurrent::Object {
   private:
    friend struct erim::thread::Object;
    inline constexpr Object() noexcept = default;

   public:
    inline constexpr Object& operator=(Object&&) noexcept      = default;
    inline constexpr Object(const Object&) noexcept            = default;
    inline constexpr Object& operator=(const Object&) noexcept = default;
    inline constexpr Object(Object&&) noexcept                 = default;
    inline constexpr ~Object() noexcept                        = default;
};

///@b  erim::thread::Object
struct erim::thread::Object {
   protected:
    static inline constexpr concurrent::Object create() noexcept { return {}; };
    inline constexpr Object(const Object&) noexcept            = default;
    inline constexpr Object& operator=(const Object&) noexcept = default;
    inline constexpr Object(Object&&) noexcept                 = default;
    inline constexpr Object& operator=(Object&&) noexcept      = default;
    inline constexpr Object() noexcept                         = default;
   public:
    inline constexpr ~Object() noexcept = default;
};

template <typename Resource, void (*CallBack)(Resource*) noexcept>
struct erim::concurrent::Task<Resource, CallBack> {
    using task_t = erim::concurrent::task_t<Resource>;
    static constexpr inline task_t callback = CallBack;
};

template <typename Resource, auto val>
requires(val == 0)
struct erim::concurrent::Task<Resource, val> {
    static constexpr inline erim::concurrent::task_t<Resource> callback = 0;
};

template <typename Resource, bool (*CallBack)(Resource)>
struct erim::concurrent::Task<Resource, CallBack> {
    static constexpr inline void callback(Resource* obj) noexcept {
        while (CallBack(obj));
    }
};

template <typename Resource, bool (Resource::*Method)()>
struct erim::concurrent::Task<Resource, Method> {
    static constexpr inline void callback(Resource* obj) noexcept {
        while ((obj->*Method)());
    }
};

template <typename Resource, void (Resource::*Method)()>
struct erim::concurrent::Task<Resource, Method> {
    static constexpr inline void callback(Resource* obj) noexcept {
        (obj->*Method)();
    }
};

// template <typename Thread, typename Resource, auto first_task>
// struct erim::thread::Pool : erim::thread::Object {
//    private:
//     using enum erim::atomic_order;
//     using task_t           = erim::concurrent::task_t<Resource>;
//     using const_atomic     = erim::atomic<const task_t>;
//     using atomic           = erim::atomic<task_t>;
//     using threads_vector_t = erim::varlen_t<Thread>;
//     struct SharedInstance : erim::concurrent::Object {
//         Pool* const pool;
//         inline constexpr void operator()() noexcept {
//             for (task_t run = pool->copy_work(); run != 0;) [[likely]] {
//                 run(&pool->resource);
//                 for (task_t old = run; (run = pool->copy_work()) == old;);
//             }
//         };
//     };

//     template <auto task>
//     static constexpr inline task_t task_v = concurrent::task_v<Resource, task>;
//     task_t task;
//     threads_vector_t threads;
//     Resource resource;
//    public:
//     static_assert(task_v<first_task> != 0, "Invalid First Task, choose other.");
//     using resource_t = Resource;
//     using threads_t  = Thread;

//     inline constexpr ~Pool() noexcept { change_work<>(); }
//     inline constexpr Pool(unsigned num, auto... vals) noexcept
//         : task{task_v<first_task>},
//           threads{num},
//           resource{Object::create(), vals...}  //
//     {
//         for (threads_t& thread : threads)
//             new (&thread) threads_t{SharedInstance{Object::create(), this}};
//     }
//     inline constexpr Resource& get_resource() noexcept { return resource; }

//     inline const task_t copy_work() const noexcept {
//         return const_atomic(task).load(acquire);
//     }
//     inline constexpr void work() noexcept { task(&resource); }

//     template <auto new_task>
//     inline constexpr void change_and_work() noexcept {
//         change_work<new_task>(), work();
//     }
//     template <auto new_task = 0>
//     inline constexpr void change_work() noexcept {
//         atomic(task).store(task_v<new_task>, release);
//     }
// };

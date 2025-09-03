module;
#include <new>
export module erim_concurrent;
import erim_primitives;

///@note,
//  when you read atomic is not std::atomic is std::atomic_ref
//  when you read atomic_order is the same as atomic order

export namespace erim
{
    template <typename T>
    struct Node {
        T item;
        Node* next = nullptr;
    };

    // excepts construction and destruction methods all methods are concurrent
    // designed to be fast and error prone, have restricted use cases
    namespace concurrent
    {
        struct JobClass;

        /// use when you @b "dont_know" how many elements the buffer needs
        template <typename T, unsigned N = 1>
        struct Buffer;

        /// use when you @b "already_know" how many elements the buffer needs
        // all threads must to push or all must to do pop
        // main thread must to act as arbitre
        template <
            typename T,
            bool must_delete   = 1,
            size_t T_alignment = alignof(T)>
        struct SizedBuffer;
    };

    //classes used for concurrency but no method is concurrent
    template <typename T>
    concept AnyJob = true;

    template <typename T>
    concept AnyThread = true;

    namespace thread
    {
        template <typename T, unsigned N>
        struct Vector;

        template <AnyJob JobClass, AnyThread ThreadType>
        struct Pool;
    };
}

template <typename T, unsigned N>
struct erim::thread::Vector {
    T (*data)[N];
    constexpr inline operator auto &() const noexcept { return data; }
    Vector(const Vector&)            = default;
    Vector& operator=(const Vector&) = default;
    Vector(Vector&&)                 = default;
    Vector& operator=(Vector&&)      = default;
    inline constexpr ~Vector() noexcept { delete[] data; };
    inline constexpr Vector() noexcept : data{new(std::nothrow) T[1][N]} {};
};

struct erim::concurrent::JobClass {
    using work_t                 = bool (JobClass::*)() noexcept;
    static constexpr work_t stop = 0;
   private:
    work_t work;
    using atomic = atomic<work_t>;
    static_assert(atomic::is_always_lock_free, "Unsupported Platform");

   public:
    constexpr inline JobClass(work_t work) noexcept : work{work} {}
    constexpr inline JobClass() noexcept : work{stop} {}
    constexpr inline ~JobClass() noexcept = default;
    JobClass(const JobClass&)             = delete;
    JobClass& operator=(const JobClass&)  = delete;
    JobClass(JobClass&&)                  = delete;
    JobClass& operator=(JobClass&&)       = delete;

    inline void change_work(work_t other) {
        atomic(work).store(other, atomic_order::release);
    }

    static inline void execute(JobClass* j) {
        auto task = atomic(j->work).load(atomic_order::acquire);
        if (task == stop) return;
        while (1) {
            while ((j->*task)());
            while (1) {
                auto other_task = atomic(j->work).load(atomic_order::acquire);
                if (other_task == task) continue;
                if (task == stop) return;
                task = other_task;
                break;
            }
        }
    }
   private:
    template <AnyJob JobClass, AnyThread ThreadType>
    friend struct erim::thread::Pool;
    inline constexpr void execute_alone() {
        while ((this->*work)() == true);
        return;
    }
};

template <erim::AnyJob JobClassDerived, erim::AnyThread ThreadType>
struct erim::thread::Pool {
    JobClassDerived job;
    ThreadType* threads;

    using work_t = bool (JobClassDerived::*)() noexcept;

    Pool(const Pool&)            = default;
    Pool& operator=(const Pool&) = default;
    Pool(Pool&&)                 = default;
    Pool& operator=(Pool&&)      = default;

    inline constexpr void work_together() { return job.execute_alone(); }
    inline constexpr void change_work(work_t cwork) {
        job.change_work(static_cast<erim::concurrent::JobClass::work_t>(cwork));
    }
    inline constexpr Pool(unsigned num, work_t cwork) noexcept
        : job{static_cast<erim::concurrent::JobClass::work_t>(cwork)},
          threads(new(std::nothrow) ThreadType[num]) {
        for (unsigned i = 0; i < num; ++i) {
            threads[i] = ThreadType(JobClassDerived::execute, &job);
        };
    }
    constexpr inline ~Pool() noexcept {
        change_work(0);
        delete[] threads;
    }
};

template <typename T, bool must_delete, size_t alignment>
struct erim::concurrent::SizedBuffer {
   private:
    ///@note
    // My atomic template is like std::atomic_ref
    // My atomic_order is std::memory_order
    using atomic = atomic<size_t>;
    using enum atomic_order;
    alignas(64) const size_t size;
    alignas(64) size_t count;
    T* items;

    SizedBuffer(const SizedBuffer&)            = delete;
    SizedBuffer& operator=(const SizedBuffer&) = delete;
    SizedBuffer(SizedBuffer&&)                 = delete;
    SizedBuffer& operator=(SizedBuffer&&)      = delete;
    static constexpr std::align_val_t items_sz{alignment > 64 ? alignment : 64};
   public:
    inline constexpr ~SizedBuffer() noexcept {
        if constexpr (must_delete) ::operator delete(items, items_sz);
    }
    inline constexpr SizedBuffer(size_t size, T* items) noexcept
        : size{size}, count{0}, items{items} {}
    inline constexpr SizedBuffer(size_t size) noexcept requires(not must_delete)
        : size{size},
          count{0},
          items{static_cast<T*>(
              ::operator new(sizeof(T) * size, items_sz, std::nothrow)
          )} {}

    inline constexpr void reset() { atomic(count).store(0, release); }

    inline constexpr void push(const T* items, const size_t num = 1) noexcept {
        const size_t idx = atomic(count).fetch_add(num, acquire);
        for (size_t i = 0; i < num; ++i) items[idx + i] = items[i];
    }

    inline constexpr T* pop() noexcept {
        const size_t idx = atomic(count).fetch_add(1, acquire);
        if (idx > size) return 0;
        return &items[idx];
    }
};

template <typename T, unsigned N>
struct erim::concurrent::Buffer {
   private:
    ///@note
    // My atomic template is like std::atomic_ref
    // My atomic_order is std::memory_order
    using Node   = erim::Node<T>;
    using atomic = atomic<Node*>;
    using enum atomic_order;
    Node* ptr;
    static constexpr auto spot = (sizeof(Node) - sizeof(T)) / sizeof(Node*);

    Buffer(const Buffer&)            = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&&)                 = delete;
    Buffer& operator=(Buffer&&)      = delete;
   public:
    using LocalVector = erim::thread::Vector<Node, N>;

    inline constexpr Buffer() noexcept = default;
    inline constexpr ~Buffer() noexcept {
        Node* tgt_0 = atomic(ptr).exchange(0, relaxed);  //its supposed to local
        Node* tgt_1;

        while (1) {
            if (!tgt_0) break;
            tgt_1 = tgt_0[N - 1].next;
            delete[] tgt_0;
            if (!tgt_1) break;
            tgt_0 = tgt_1[N - 1].next;
            delete[] tgt_1;
        }
    }

    /// @brief,
    //  Keep Searching the item until found or acquire the lock at end of Chain,
    //  put the item there then release the lock, assert storage is thread_safe.
    inline constexpr bool push(const T& item, LocalVector& storage) noexcept {
        Node** next    = &ptr;
        Node* expected = 0;
        size_t jmps    = 0;
        if (!storage && !(storage = LocalVector())) return false;
        while (true) {
            if (next[0]->item == item) return true;
            while (true) {
                if (atomic(next[0]).compare_exchange_strong(
                        expected, (Node*)UINTPTR_MAX, acq_rel, relaxed
                    )) {
                    if (jmps %= N) [[likely]] {
                        next[spot]->item = item;
                        atomic(next[0]).store(next[spot], release);
                    }
                    else {
                        storage[0][0].item = item;
                        atomic(next[0]).store(storage[0], release);
                        storage = LocalVector();
                    }
                    return true;
                }
                else if (expected != (Node*)UINTPTR_MAX) [[likely]] {
                    next     = &expected->next;
                    expected = nullptr;
                    ++jmps;
                    break;
                }
            }
        }
    }
};

// concurrent::Buffer<int, 4> a;
// thread::Vector<Node<int>, 4> buf;
// void aj() { a.push(1, buf); }

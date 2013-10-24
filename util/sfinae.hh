#include <type_traits>

#ifndef _SCRATCH_SFINAE
#define _SCRATCH_SFINAE

#define CREATE_FUNCTION_TEST(symbol)                                    \
    template <class Ret, class... Args>                                 \
    struct detect_fn_##symbol                                           \
    {                                                                   \
        // Name a dummy function with an undesired type                 \
        using DummyFn = typename std::conditional<                      \
            std::is_same<Ret, void>::value, \
            int, void>::type (*)(); \
        // DummyFn symbol;                                              \
        template <class R = decltype(symbol(std::declval<Args>()...))>  \
        static typename std::enable_if<                             \
            std::is_same<typename std::decay<Ret>::type, R>::value, \
            std::true_type>::type test(int); \
        static std::false_type test(...);                               \
        static const bool value = decltype(test(0))::value;             \
    }

#define CREATE_MEMBER_TEST(symbol)                               \
    template <class T, class U>                                         \
    struct detect_mem_##symbol                                         \
    {                                                                   \
        template<class T_ = T>                                          \
        static typename std::enable_if<                                 \
            std::is_same<typename std::decay<U>::type,\
                         decltype(T_::symbol)>::value,  \
            std::true_type>::type test(int); \
        static std::false_type test(...);                               \
        static const bool value = decltype(test(0))::value;             \
    }

#define CREATE_MEMBER_FUNCTION_TEST(symbol)                             \
    template <class T, class Ret, class... Args>                        \
    struct detect_mem_fn_##symbol                                       \
    {                                                                   \
    template <class T_ = T,                                             \
              class R = decltype(                                       \
                  std::declval<T_>().symbol(std::declval<Args>()...))>  \
    static typename std::enable_if<                                 \
		std::is_class<T_>::value && \
		std::is_same<typename std::decay<Ret>::type, R>::value, \
		std::true_type>::type test(int); \
    static std::false_type test(...);                                   \
    static const bool value = decltype(test(0))::value;                 \
}


#ifdef _BUILD_TEST

struct FooX { char name(); int add(int);};
struct FooY { static char name; };
struct FooZ { };

CREATE_MEMBER_TEST(name);

static_assert(detect_mem_name<FooY, char>::value, "");
static_assert(!detect_mem_name<FooX, char>::value, "");
static_assert(!detect_mem_name<FooZ, char>::value, "");


CREATE_MEMBER_FUNCTION_TEST(name);

static_assert(detect_mem_fn_name<FooX, char>::value, "");
static_assert(!detect_mem_fn_name<FooY, char>::value, "");
static_assert(!detect_mem_fn_name<FooZ, char>::value, "");

#endif

#endif


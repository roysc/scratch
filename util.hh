#include <tuple>
#include <type_traits>
#include <initializer_list>

#include <string>
#include <iostream>
#include <sstream>

#ifndef _SCRATCH_UTIL
#define _SCRATCH_UTIL

#define CREATE_FUNCTION_TEST(symbol)                                    \
    template <class Ret, class... Args>                                 \
    struct detect_fn_##symbol                                           \
    {                                                                   \
        using DummyFn = typename std::conditional<                      \
            std::is_same<Ret, void>::value, \
            int, void>::type (*)(); \
        DummyFn symbol;                                                 \
        template <class R = decltype(symbol(std::declval<Args>()...))>  \
        static typename std::enable_if<                             \
            std::is_same<typename std::decay<Ret>::type, R>::value, \
            std::true_type>::type test(int); \
        static std::false_type test(...);                               \
        static const bool value = decltype(test(0))::value;             \
    };

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
    };

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
};


namespace util
{
    namespace meta
    {
        /* ∀ T ∈ Ts where Pred<T> */
        template <template <class> class, class...>
        struct all_satisfy;
        template <template <class> class Pred>
        struct all_satisfy<Pred> : public std::true_type
        {};
        template <template <class> class Pred, class T, class... Ts>
        struct all_satisfy<Pred, T, Ts...>
            : public std::conditional<Pred<T>::value,
                                      all_satisfy<Pred, Ts...>,
                                      std::false_type>::type
        {};

        /* ∃ T ∈ Ts where Pred<T> */
        template <template <class> class, class...>
        struct any_satisfy;
        template <template <class> class Pred>
        struct any_satisfy<Pred> : public std::false_type
        {};
        template <template <class> class Pred, class T, class... Ts>
        struct any_satisfy<Pred, T, Ts...>
            : public std::conditional<Pred<T>::value,
                                      std::true_type,
                                      any_satisfy<Pred, Ts...> >::type
        {};

        /// Check membership in Ts 
        template <class T, class... Ts>
        struct is_member
        {
            template <class T_> struct predicate : public std::is_same<T, T_> {};
            static constexpr bool value = any_satisfy<predicate, Ts...>::value;
        };
        
        template <class T, template <class...> class Variadic, class... Ts>
        struct is_member<T, Variadic<Ts...> > : is_member<T, Ts...>
        {};
    }

    namespace meta
    {
        /** Filter a parameter pack */    
        template <template <class> class, class> struct FilterImpl;

        template <template <class> class Pred, template <class...> class Variadic>
        struct FilterImpl<Pred, Variadic<> > { using type = Variadic<>; };
        template <template <class> class Pred,
                  template <class...> class Variadic,
                  class T, class... Ts>
        struct FilterImpl<Pred, Variadic<T, Ts...> >
        {
            template <class, class> struct Cons;
            template <class Head, class... Tail>
            struct Cons<Head, Variadic<Tail...> >
            { using type = Variadic<Head, Tail...>; };
        
            using type = typename std::conditional<
                Pred<T>::value,
                typename Cons<T, typename FilterImpl<Pred, Variadic<Ts...> >::type>::type,
                typename FilterImpl<Pred, Variadic<Ts...> >::type
                >::type;
        };

        template <template <class> class Pred, class Variadic>
        using Filter = typename FilterImpl<Pred, Variadic>::type;

    
        /** Transform a variadic template */
        template <template <class> class, class> struct TransformImpl;

        template <template <class> class How,
                  template <class...> class Variadic, class... Ts>
        struct TransformImpl<How, Variadic<Ts...>>
        { using type = Variadic<How<Ts>...>; };
    
        template <template <class> class How, class Variadic>
        using Transform = typename TransformImpl<How, Variadic>::type;
    }

    using namespace meta;
    
    /** Enable lookup by type (until C++14 std::get) */
    template <class, class...> struct index_of;

    template <class T, class... Ts>
    struct index_of<T, T, Ts...>
        : public std::integral_constant<int, 0>
    {};
    template <class T, class U, class... Ts>
    struct index_of<T, U, Ts...>
        : public std::integral_constant<int, 1 + index_of<T, Ts...>::value>
        // , public std::enable_if<is_member<T, Ts...>::value>
    {};

    template <class, class> struct index_within;
    
    template <class T, template <class...> class Variadic, class... Ts>
    struct index_within<T, Variadic<Ts...> > : index_of<T, Ts...> {};
    
    /** Type container */
    template <class... Ts>
    using TypeVector = std::tuple<Ts...>;

    template <class... Ts>
    struct TypeSet : public TypeVector<Ts...>
    {
        template <class> struct contains;
        template <class U>
        struct contains : is_member<U, Ts...> {};
        template <class... Us>
        struct contains<TypeSet<Us...> > : all_satisfy<contains, Us...>
        {};
    };
    
    template <class Variadic>
    using variadic_size = std::tuple_size<Variadic>;

    /** Expand variadic template types 
     *  http://loungecpp.wikidot.com/tips-and-tricks%3aindices
     */
    template <size_t... Is>
    struct indices {};

    template <size_t N, size_t... Is>
    struct _build_indices : _build_indices<N-1, N-1, Is...> {};
 
    template <size_t... Is>
    struct _build_indices<0, Is...> : indices<Is...> {};
 
    template <class Variadic>
    struct _indices_for;

    template <template <class...> class Variadic, class... Ts>
    struct _indices_for<Variadic<Ts...> >
    { using type = _build_indices<sizeof...(Ts)>; };
    
    template <class Variadic>
    using indices_for = typename
        _indices_for<typename std::remove_reference<Variadic>::type>::type;
    

    // until C++14
    template <class T, class... Ts>
    constexpr T& get(std::tuple<Ts...>& t)
    { return std::get<index_of<T, Ts...>::value>(t); }

    template <class T, class... Ts>
    constexpr T&& get(std::tuple<Ts...>&& t)
    { return std::forward<T&&>(get<T>(t)); }

    
    // convenience stuff
    // until C++14
    template <class T>
    using add_pointer_t = typename std::add_pointer<T>::type;

    template <bool B, class T = void>
    using enable_if_t = typename std::enable_if<B, T>::type;
    
    /** Static-for functions */
    // using boost::mpl::for_each;

    // using swallow = int[];
    struct swallow { swallow(std::initializer_list<int>) {} };

    template <class Variadic, class Functor, size_t... Is>
    void _expand_apply(Functor&& f, Variadic&& tuple, indices<Is...>)
    { swallow { (f(std::get<Is>(tuple)), 0)... }; }

    template <class Variadic, class Functor>
    void expand_apply(Functor&& f, Variadic&& tuple)
    { _expand_apply(f, tuple, indices_for<Variadic> {}); }

    template <class Variadic, class Functor>
    void expand_apply(Functor&& f)
    { expand_apply(f, Variadic()); }

    
    template <class T, class... Args>
    auto make(Args&&... args)
        -> decltype(T {std::forward<Args>(args)...}) // use declval here?
    { return T {std::forward<Args>(args)...}; }

    // string stuff

    
    namespace io
    {
        template <class... Args>
        std::ostream& print_to(std::ostream& out, Args&&... args)
        {
            swallow {(out << std::forward<Args>(args), 0)...};
            return out;
        }

        template <class... Args>
        std::ostream& print(Args&&... args)
        { return print_to(std::cout, std::forward<Args>(args)...); }

        template <class... Args>
        std::ostream& println(Args&&... args)
        { return print(std::forward<Args>(args)..., "\n"); }

        
        template <class Tuple, size_t... Is>
        void _print_tuple(std::ostream& out, const Tuple& t, indices<Is...>)
        {
            out << "(";
            swallow {(
                out << (Is == 0 ? "" : ", ") << std::get<Is>(t),
            0)...};
            out << ")";
        }

        template <class... Args>
        std::ostream& operator<<(std::ostream& out,
                                 const std::tuple<Args...>& t)
        { _print_tuple(out, t, indices_for<std::tuple<Args...> > {}); }

        
        CREATE_MEMBER_FUNCTION_TEST(to_string);
        template <class T>
        using has_to_string = detect_mem_fn_to_string<T, std::string>;
        
        template <class T>
        enable_if_t<has_to_string<T>::value,
                    std::string>
        to_string(T& a)
        { return a.to_string(); }

        // template <class T, bool E = has_to_string<T>::value>
        // std::string to_string(T&& a)
        // { return a.to_string(); }

        // template <class T>
        // enable_if_t<!has_to_string<T>::value, std::string>
        // to_string(T&& a)
        // { return std::to_string(std::forward<T>(a)); }

        
        
        // the below is perhaps a bit too clever...
        
        // CREATE_FUNCTION_TEST(to_string);
        // template <class T>
        // using can_to_string = detect_fn_to_string<std::string, T&>;
        
        // template <class T>
        // util::enable_if_t<
        //     can_to_string<T>::value,
        //     std::ostream&>
        // operator<<(std::ostream& out, T& a)
        // {
        //     return out << to_string(a);
        // }

        // template <class T>
        // std::ostream& operator<<(std::ostream& out, T& a)
        // {
        //     return out << to_string(a);
        // }

    }

    using namespace io;
}

#ifdef _BUILD_TEST

// #include <iostream>
// #include <typeinfo>

int fn1(int) { return 1; }
void fn2() {}

using namespace util;

// static_assert(false, "working?");
static_assert(any_satisfy<std::is_integral, int, float>::value, "");
static_assert(!any_satisfy<std::is_integral, double, float>::value, "");
    
static_assert(all_satisfy<std::is_integral, int, long, unsigned>::value, "");
static_assert(!all_satisfy<std::is_integral, int, float>::value, "");
static_assert(all_satisfy<std::is_function, decltype(fn1), decltype(fn2)>::value, "");

static_assert(is_member<int, long, char, int>::value, "");
static_assert(!is_member<int, double, long, float>::value, "");
    
static_assert(std::is_same<
              Filter<std::is_integral, std::tuple<int, float, long> >,
              std::tuple<int, long> >::value, "");

static_assert(std::is_same<
              Transform<add_pointer_t, std::tuple<int, float, char*> >,
              std::tuple<int*, float*, char**> >::value, "");

struct FooX { char name(); int add(int);};
struct FooY { static char name; };
struct FooZ { };

// CREATE_STATIC_MEMBER_TEST(name);

// static_assert(detect_mem_name<FooY, char>::value, "");
// static_assert(!detect_mem_name<FooX, char>::value, "");
// static_assert(!detect_mem_name<FooZ, char>::value, "");


CREATE_MEMBER_FUNCTION_TEST(name);

static_assert(detect_mem_fn_name<FooX, char>::value, "");
static_assert(!detect_mem_fn_name<FooY, char>::value, "");
static_assert(!detect_mem_fn_name<FooZ, char>::value, "");


struct _S { std::string to_string() { return std::string("ess"); }};

// void test_to_string()
int main()
{
    // using util::io::to_string;
    _S s;
    auto r = to_string(s);
    std::cout << r << '\n';
    println(s);
}


#endif

#endif

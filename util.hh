#include <tuple>
#include <type_traits>
#include <initializer_list>
#include <iterator>

#include <sstream>
#include <iostream>
#include <string>

#include "cxx14.hh"
#include "sfinae.hh"

#ifndef _SCRATCH_UTIL
#define _SCRATCH_UTIL

namespace util
{
    namespace traits
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

        
        template <class Pred>
        using neg = std::integral_constant<bool, !Pred::value>;
        
        template <class T>
        struct is_iterable
        {
            template <class T_ = std::decay_t<T>,
                      class B = decltype(std::begin(std::declval<T_>())),
                      class E = decltype(std::end(std::declval<T_>()))> 
            static std::enable_if_t<
                std::is_same<B, E>::value,
                std::true_type> test(int);
            static std::false_type test(...);
            static const bool value = decltype(test(0))::value;
        };
    }

    using namespace traits;

    
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
    
    
    /** Enable lookup by type (until C++14 std::get) */
    template <class, class...> struct index_of;

    template <class T, class... Ts>
    struct index_of<T, T, Ts...>
        : public std::integral_constant<int, 0>
    {};
    template <class T, class U, class... Ts>
    struct index_of<T, U, Ts...>
        : public std::integral_constant<int, 1 + index_of<T, Ts...>::value>
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

    template <class T, class... Ts>
    constexpr const T& get(const std::tuple<Ts...>& t)
    { return std::get<index_of<T, Ts...>::value>(t); }

    
    
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
        { return print(std::forward<Args>(args)..., '\n'); }

        
        template <class Tuple, size_t... Is>
        void _print_tuple(std::ostream& out, const Tuple& t, indices<Is...>)
        {
            out << "(";
            swallow {(
                out << (Is == 0 ? "" : ", ") << std::get<Is>(t),
            0)...};
            out << ")";
        }

        template <class... Ts>
        std::ostream& operator<<(std::ostream& out,
                                 const std::tuple<Ts...>& t)
        { _print_tuple(out, t, indices_for<std::tuple<Ts...> > {}); }


        template <class T>
        struct is_string
        {
            template <class T_ = std::decay_t<T>,
                      class Val = typename std::iterator_traits<
                          decltype(std::begin(std::declval<T_>()))
                          >::value_type>
            static std::enable_if_t<
                std::is_same<T_, std::basic_string<Val> >::value,
                std::true_type> test(int);
            static std::false_type test(...);
            static const bool value = decltype(test(0))::value;
        };

        
        template <class C,
                  class = std::enable_if_t<
                      is_iterable<C>::value &&
                      !is_string<C>::value> >
        std::ostream& operator<<(std::ostream& out, const C& c)
        {
            out << '[';
            bool first = true;
            for (auto it = std::begin(c); it != std::end(c); ++it) {
                if (!first) out << ", ";
                out << *it;
                first = false;
            }
            out << ']';
        }
        
        CREATE_MEMBER_FUNCTION_TEST(to_string);
        template <class T>
        using has_to_string = detect_mem_fn_to_string<
            typename std::decay<T>::type, std::string>;
        
        template <class T,
                  class = std::enable_if_t<has_to_string<T>::value> >
        std::string to_string(T&& a)
        { return a.to_string(); }
        
        
        // CREATE_FUNCTION_TEST(to_string);
        // template <class T>
        // using can_to_string = detect_fn_to_string<std::string, T&>;
        
        template <class T,
                  class = std::enable_if_t<has_to_string<T>::value> >
        std::ostream& operator<<(std::ostream& out, T&& a)
        { return out << to_string(a); }

    }

    using namespace io;
}

#ifdef _BUILD_TEST

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
              Transform<std::add_pointer_t, std::tuple<int, float, char*> >,
              std::tuple<int*, float*, char**> >::value, "");


// void test_to_string()


#endif

#endif

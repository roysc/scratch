#include <type_traits>
#include <tuple>

#include <boost/mpl/vector.hpp>

#ifndef _SCRATCH_UTIL_
#define _SCRATCH_UTIL_

namespace util
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

    
    /** Filter a parameter pack */    
    template <template <class> class, class> struct filter;

    template <template <class> class Pred, template <class...> class Variadic>
    struct filter<Pred, Variadic<> > { using type = Variadic<>; };
    template <template <class> class Pred,
              template <class...> class Variadic,
              class T, class... Ts>
    struct filter<Pred, Variadic<T, Ts...> >
    {
        template <class, class> struct Cons;
        template <class Head, class... Tail>
        struct Cons<Head, Variadic<Tail...> >
        { using type = Variadic<Head, Tail...>; };
        
        using type = typename std::conditional<
            Pred<T>::value,
            typename Cons<T, typename filter<Pred, Variadic<Ts...> >::type>::type,
            typename filter<Pred, Variadic<Ts...> >::type
            >::type;
    };

    
    /** Transform a variadic template */
    template <template <class> class, class> struct transform;

    template <template <class> class How,
              template <class...> class Variadic, class... Ts>
    struct transform<How, Variadic<Ts...>>
    { using type = Variadic<How<Ts>...>; };

    
    /** Enable lookup by type (will be in std::get by C++14) */
    template <class, class...> struct _index_of;

    template <class T, class... Ts>
    struct _index_of<T, T, Ts...>
        : public std::integral_constant<int, 0>
    {};
    template <class T, class U, class... Ts>
    struct _index_of<T, U, Ts...>
        : public std::integral_constant<int, 1 + _index_of<T, Ts...>::value>
    {};

    
    /** A tuple with indexing by type */
    template <class... Ts>
    struct TypeVector : private std::tuple<Ts...>
                      , public boost::mpl::vector<Ts...>
    {
        template <class T> T&& get() {
            return std::get<_index_of<T>::value>(*this);
        }
        template <class T> void set(T&& a) {
            std::get<_index_of<T>::value>(*this) = a;
        }
    };
}

#ifdef _BUILD_TEST

// #include <iostream>
// #include <typeinfo>

int f(int) { return 1; }
void g() {}

using namespace util;
    
// static_assert(false, "working?");
static_assert(any_satisfy<std::is_integral, int, float>::value, "");
static_assert(!any_satisfy<std::is_integral, double, float>::value, "");
    
static_assert(all_satisfy<std::is_integral, int, long, unsigned>::value, "");
static_assert(!all_satisfy<std::is_integral, int, float>::value, "");
static_assert(all_satisfy<std::is_function, decltype(f), decltype(g)>::value, "");

static_assert(is_member<int, long, char, int>::value, "");
static_assert(!is_member<int, double, long, float>::value, "");
    
static_assert(std::is_same<
              filter<std::is_integral, std::tuple<int, float, long> >::type,
              std::tuple<int, long> >::value, "");

namespace std
{
    template <class T>
    using add_pointer_t = typename add_pointer<T>::type;
}

static_assert(std::is_same<
              transform<std::add_pointer_t,
              std::tuple<int, float, char*> >::type,
              std::tuple<int*, float*, char**> >::value, "");

#endif

#endif

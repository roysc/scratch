#include <type_traits>
#include <tuple>

#ifndef _SCRATCH_UTIL
#define _SCRATCH_UTIL

namespace util
{
    
    /* ∀ T ∈ Ts where Pred<T> */
    template <template <class> class, class...>
    struct all_satisfy;
    template <template <class> class Pred, class T>
    struct all_satisfy<Pred, T> : Pred<T>
    {};
    template <template <class> class Pred, class T, class... Ts>
    struct all_satisfy<Pred, T, Ts...>
        : std::conditional<Pred<T>::value,
                           all_satisfy<Pred, Ts...>,
                           std::false_type>::type
    {};


    /* ∃ T ∈ Ts where Pred<T> */
    template <template <class> class, class...>
    struct any_satisfy;
    template <template <class> class Pred, class T>
    struct any_satisfy<Pred, T> : Pred<T>
    {};
    template <template <class> class Pred, class T, class... Ts>
    struct any_satisfy<Pred, T, Ts...>
        : std::conditional<Pred<T>::value,
                           std::true_type,
                           any_satisfy<Pred, Ts...> >::type
    {};


    template <class T, class... Ts>
    struct is_member
    {
        template <class T_> struct predicate : std::is_same<T, T_> {};
        static constexpr bool value = any_satisfy<predicate, Ts...>::value;
    };
    
    template <template <class> class, template <class...> class, class...>
    struct filter;
    template <template <class> class Pred, template <class...> class Tuple>
    struct filter<Pred, Tuple> : Tuple<>
    {};
    template <template <class> class Pred,
              template <class...> class Tuple,
              class T, class... Ts>
    struct filter<Pred, Tuple, T, Ts...>
    {
        template <class U, class... Us> struct prepend : Tuple<U, Us...> {};
        using type = typename
            std::conditional<Pred<T>::value,
                             prepend<T, Ts...>,
                             filter<Pred, Tuple, Ts...>>::type;
    };
    
    
    /** Enable lookup by type */
    template <class, class...> struct _index_of;
    template <class T, class... Ts>
    struct _index_of<T, T, Ts...> : std::integral_constant<int, 0> {};

    template <class T, class U, class... Ts>
    struct _index_of<T, U, Ts...>
        : std::integral_constant<int, 1 + _index_of<T, Ts...>::value> {};


    /** A tuple with indexing by type */
    template <class... Ts>
    struct TypeVector : std::tuple<Ts...>
    {
        template <class T> T& get() {
            return std::get<_index_of<T>::value>(*this);
        }
        template <class T> void set(T& a) {
            std::get<_index_of<T>::value>(*this) = a;
        }
    };
}

#ifdef _BUILD_TEST

int main() {
    using namespace util;
    

    // static_assert(false, "working?");
    static_assert(any_satisfy<std::is_integral, int, float>::value, "");
    static_assert(!any_satisfy<std::is_integral, double, float>::value, "");
    
    static_assert(all_satisfy<std::is_integral, int, long, unsigned>::value, "");
    int f(int) { return 1; }
    void g() {}
    static_assert(!all_satisfy<std::is_integral, int, float>::value, "");
    static_assert(
        all_satisfy<std::is_function, decltype(f), decltype(g)>::value, "");

    static_assert(is_member<int, long, char, int>::value, "");
    static_assert(!is_member<int, double, long, float>::value, "");

    static_assert(std::is_same<
                  filter<std::is_integral, std::tuple, int, float, long>>,
                  std::tuple<int, long>>,
        "");
}
#endif

#endif

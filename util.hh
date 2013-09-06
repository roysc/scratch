#include <type_traits>
#include <tuple>
#include <initializer_list>

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
    
    template <template <class> class How, class Variadic>
    using transform_t = typename transform<How, Variadic>::type;
    
    
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
    
    /** A tuple with indexing by type */
    // template <class... Ts>
    // struct TypeVector : public std::tuple<Ts...>
    // { };

    template <class... Ts>
    using TypeVector = typename std::tuple<Ts...>;

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

    
    
    template <class T>
    using add_pointer_t = typename std::add_pointer<T>::type;

    
    /** Static-for functions */
    // using boost::mpl::for_each;

    // for convenience
    using ignore = int[];
    // struct ignore { ignore(std::initializer_list<int>); };

    template <class Variadic, class Functor, size_t... Is>
    void _expand_apply(Functor&& f, Variadic&& tuple, indices<Is...>)
    { ignore { (f(std::get<Is>(tuple)), 0)... }; }

    template <class Variadic, class Functor>
    void expand_apply(Functor&& f, Variadic&& tuple)
    { _expand_apply(f, tuple, indices_for<Variadic> {}); }

    template <class Variadic, class Functor>
    void expand_apply(Functor&& f)
    { expand_apply(f, Variadic()); }

    
    // By C++14, such functors can be replaced with e.g.:
    // expand_apply<EntityIndex>(
    //     [&] <class Cpt> (Cpt _) {
    //         auto sub = self.m_subsystems.template get<Cpt>();
    //         entity.add_component(sub.create(src));
    // ...
    //     [&] <class Subsystem> (Subsystem _) {
    //         m_subsystems.template get<Subsystem>().update();
    //     });
    template <class Functor, class... Args>
    auto make(Args&&... args)
        -> decltype(Functor {std::forward<Args>(args)...})
    { return Functor {std::forward<Args>(args)...}; }

}

#define DECLARE_SFINAE_TEST(symbol)                                     \
    namespace util                                                      \
    {                                                                   \
        template <typename T>                                           \
            struct sfinae_has_##symbol                                  \
        {                                                               \
            using Yea = char;                                           \
            using Nay = long;                                           \
                                                                        \
            template <class U> static Yea test(decltype(&U::symbol));   \
            template <class> static Nay test(...);                      \
                                                                        \
            static constexpr bool value =                               \
                sizeof(test<T>(nullptr)) == sizeof(Yea);                \
        };                                                              \
    }



#ifdef _BUILD_TEST

// #include <iostream>
// #include <typeinfo>

int _f(int) { return 1; }
void _g() {}

using namespace util;

// static_assert(false, "working?");
static_assert(any_satisfy<std::is_integral, int, float>::value, "");
static_assert(!any_satisfy<std::is_integral, double, float>::value, "");
    
static_assert(all_satisfy<std::is_integral, int, long, unsigned>::value, "");
static_assert(!all_satisfy<std::is_integral, int, float>::value, "");
static_assert(all_satisfy<std::is_function, decltype(_f), decltype(_g)>::value, "");

static_assert(is_member<int, long, char, int>::value, "");
static_assert(!is_member<int, double, long, float>::value, "");
    
static_assert(std::is_same<
              filter<std::is_integral, std::tuple<int, float, long> >::type,
              std::tuple<int, long> >::value, "");

static_assert(std::is_same<
              transform<add_pointer_t,
              std::tuple<int, float, char*> >::type,
              std::tuple<int*, float*, char**> >::value, "");

struct _Y {};
struct _X { char name() { return 'X'; } };

DECLARE_SFINAE_TEST(name);

static_assert(sfinae_has_name<_X>::value, "");
static_assert(!sfinae_has_name<_Y>::value, "");

#endif

#endif

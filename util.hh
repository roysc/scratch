#include <type_traits>
#include <tuple>

#include <boost/mpl/vector.hpp>
#include <boost/mpl/for_each.hpp>

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
    struct TypeVector : public std::tuple<Ts...>
                      , public boost::mpl::vector<Ts...>
    { };


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
    using indices_for = typename _indices_for<Variadic>::type;
    

    // until C++14
    template <class T, class... Ts>
    T& get(std::tuple<Ts...>& tv)
    { return std::get<_index_of<T, Ts...>::value>(tv); }
    
    template <class T>
    using add_pointer_t = typename std::add_pointer<T>::type;

}


namespace functor
{
    // By C++14, this can be replaced with e.g.:
    // for_each<EntityIndex>(
    //     [&] <class Cpt> (Cpt _) {
    //         auto sub = self.m_subsystems.template get<Cpt>();
    //         entity.add_component(sub.create(src));
    // ...
    //     [&] <class Subsystem> (Subsystem _) {
    //         m_subsystems.template get<Subsystem>().update();
    //     });

    // // does not work for constructors with diff. arity than tpl. params
    // template <template <class...> class Functor, class... Args>
    // Functor<Args...> make(Args... args) {
    //     return Functor<Args...> {std::forward(args)...};
    // }
    using boost::mpl::for_each;
    
    template <class ComponentSpace, class Entity>
    struct InitComponent;

    template <class Entity>
    struct VerifyComponent
    {
        Entity self;
        
        template <class Cpt>
        void operator()(Cpt _)
        {
            auto cpt_ptr = self->template get_component<Cpt>();
            assert(cpt_ptr != nullptr &&
                   "Component dependency is not initialized!\n");
        }
    };

    template <class ComponentSpace>
    struct UpdateSubsystem
    {
        ComponentSpace self;

        template <class Subsystem>
        void operator()(Subsystem _)
        {
            self->template get_subsystem<Subsystem>().update();
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

static_assert(std::is_same<
              transform<add_pointer_t,
              std::tuple<int, float, char*> >::type,
              std::tuple<int*, float*, char**> >::value, "");

#endif

#endif

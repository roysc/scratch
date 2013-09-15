

#include "util/common.hh"

#ifndef _SCRATCH_UTIL_MP
#define _SCRATCH_UTIL_MP

namespace util
{
    namespace mp
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


        /** Enable lookup by type */
        // directly in parameter pack
        template <class, class...> struct index_of;
        // within a variadic class
        template <class, class> struct index_within;

        template <class T, class... Ts>
        struct index_of<T, T, Ts...>
            : public std::integral_constant<int, 0>
        {};
        template <class T, class U, class... Ts>
        struct index_of<T, U, Ts...>
            : public std::integral_constant<int, 1 + index_of<T, Ts...>::value>
        {};
    
        template <class T, template <class...> class Variadic, class... Ts>
        struct index_within<T, Variadic<Ts...> > : index_of<T, Ts...> {};

    
        // should perhaps go in C++14 std:: hacks
        template <class T, class... Ts>
        constexpr T& get(std::tuple<Ts...>& t)
        { return std::get<index_of<T, Ts...>::value>(t); }

        template <class T, class... Ts>
        constexpr T&& get(std::tuple<Ts...>&& t)
        { return std::forward<T&&>(get<T>(t)); }

        template <class T, class... Ts>
        constexpr const T& get(const std::tuple<Ts...>& t)
        { return std::get<index_of<T, Ts...>::value>(t); }


    
        /** Type container */
        template <class... Ts>
        using TypeVector = std::tuple<Ts...>;

        // test for subset
        template <class...> struct TypeSet;
    
        template <class... Ts>
        struct TypeSet //: public TypeVector<Ts...>
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
 

    
        /** Static-for functions */
        template <class Variadic, class Functor, size_t... Is>
        void _expand_apply(Functor&& f, Variadic&& tuple, indices<Is...>)
        { swallow { (f(std::get<Is>(tuple)), 0)... }; }

        template <class Variadic, class Functor>
        void expand_apply(Functor&& f, Variadic&& tuple)
        { _expand_apply(f, tuple, indices_for<Variadic> {}); }

        template <class Variadic, class Functor>
        void expand_apply(Functor&& f)
        { expand_apply(f, Variadic()); }

    }
    
    using namespace mp;
}

#endif

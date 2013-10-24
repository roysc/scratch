// #include "range/traits.hh"

#ifndef _SCRATCH_UTIL_TRAITS_IMPL
#define _SCRATCH_UTIL_TRAITS_IMPL

namespace util
{
namespace traits
{
template <template <class> class Pred>
struct all_satisfy<Pred> : public std::true_type
{};
template <template <class> class Pred, class T, class... Ts>
struct all_satisfy<Pred, T, Ts...>
  : public std::conditional<Pred<T>::value,
                            all_satisfy<Pred, Ts...>,
                            std::false_type>::type
{};
    
template <template <class> class Pred>
struct any_satisfy<Pred> : public std::false_type
{};
template <template <class> class Pred, class T, class... Ts>
struct any_satisfy<Pred, T, Ts...>
  : public std::conditional<Pred<T>::value,
                            std::true_type,
                            any_satisfy<Pred, Ts...> >::type
{};

template <class T, class... Ts>
struct is_member
{
  template <class T_> struct predicate : public std::is_same<T, T_> {};
  static constexpr bool value = any_satisfy<predicate, Ts...>::value;
};
    
template <class T, template <class...> class Variadic, class... Ts>
struct is_member<T, Variadic<Ts...> > : is_member<T, Ts...>
{};

    
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
}

#endif

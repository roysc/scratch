#ifndef UTIL_TRAITS_HH
#define UTIL_TRAITS_HH

#include <type_traits>
#include <iterator>

#include "std14_compat.hh"

namespace util {
namespace traits {

/** ∀ T ∈ Ts where pred<T> */
template <template <class> class, class...>
struct all_satisfy;

/** ∃ T ∈ Ts where pred<T> */
template <template <class> class, class...>
struct any_satisfy;

/// Check membership in Ts
template <class, class...>
struct is_member;

template <class pred>
using neg = std::integral_constant<bool, !pred::value>;

template <class>
struct is_iterable;

////////////////////////////////////////
// Implementation

template <template <class> class pred>
struct all_satisfy<pred> : public std::true_type
{};
template <template <class> class pred, class T, class... Ts>
struct all_satisfy<pred, T, Ts...>
  : public std::conditional<pred<T>::value,
                            all_satisfy<pred, Ts...>,
                            std::false_type>::type
{};

template <template <class> class pred>
struct any_satisfy<pred> : public std::false_type
{};
template <template <class> class pred, class T, class... Ts>
struct any_satisfy<pred, T, Ts...>
  : public std::conditional<pred<T>::value,
                            std::true_type,
                            any_satisfy<pred, Ts...> >::type
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

using namespace traits;
}

#endif

/****************************************
 ** \file       range/traits.hh
 ** \brief      Traits for ranges
 ** 
 ** \author     Roy Crihfield
 **/

#ifndef UTIL_RANGE_TRAITS_HH
#define UTIL_RANGE_TRAITS_HH

#include <type_traits>

namespace range {
namespace traits {

template <class T>
struct is_forward_range
{
  template <class T_ = std::decay_t<T>,
            class EltRef = decltype(std::declval<T_>().front()),
            class HasNext = decltype(std::declval<T_>().has_next()),
            class PopFront = decltype(std::declval<T_>().pop_front())>
  static std::enable_if_t<
    // std::is_lvalue_reference<EltRef>::value &&
    std::is_same<HasNext, bool>::value,
    std::true_type> test(int);
  static std::false_type test(...);
  static const bool value = decltype(test(0))::value;
};

template <class T>
using is_range = std::integral_constant<
  bool,
  std::is_base_of<BasicRange, T>::value |
  is_forward_range<T>::value
  >;
}

using namespace traits;
}

#endif
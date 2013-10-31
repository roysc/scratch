#include <type_traits>

#ifndef UTIL_STD14_COMPAT_HH
#define UTIL_STD14_COMPAT_HH

namespace std
{
// convenient stuff coming in C++14
template <class T>
using add_pointer_t = typename add_pointer<T>::type;

template <bool B, class T = void>
using enable_if_t = typename enable_if<B, T>::type;

template <class T>
using decay_t = typename decay<T>::type;


}

#endif

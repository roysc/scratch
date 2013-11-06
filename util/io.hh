/****************************************
 ** \file       util/io.hh
 ** \brief      IO utilities
 */

#ifndef UTIL_IO_HH
#define UTIL_IO_HH

#include <sstream>
#include <iostream>
#include <string>
#include <iterator>

#include <boost/format.hpp>     // oof

#include "sfinae.hh"
#include "common.hh"
#include "traits.hh"
#include "mp.hh"

namespace util {
namespace io {

template <class CharT, class Traits, class... Args>
std::basic_string<CharT, Traits>
format(std::basic_string<CharT, Traits> spec, Args&&... args)
{
  boost::format fmt(spec);
  swallow {(fmt = fmt % args, 0)...};
  return fmt.str();
}

template <class T>
using is_char = is_member<T, char, wchar_t, char16_t, char32_t>;

template <class T>
struct is_string
{
  template <class T_ = std::decay_t<T>,
            class Elem = std::decay_t<
              typename std::iterator_traits<
                decltype(std::begin(std::declval<T_>()))
                >::value_type> > 
  static std::enable_if_t<
    is_char<Elem>::value &
    std::is_same<T_, std::basic_string<Elem> >::value,
    std::true_type> test(int);
  static std::false_type test(...);
  static const bool value = decltype(test(0))::value;
};

template <class T, class CharT, class Traits>
void
print_iterable(std::basic_ostream<CharT, Traits>& out, const T& c)
{
  out << '[';
  bool first = true;
  for (auto it = std::begin(c); it != std::end(c); ++it)
  {
    if (!first) out << ", ";
    out << *it;
    first = false;
  }
  out << ']';
}

template <class C, class CharT, class Traits,
          class = std::enable_if_t<
            traits::is_iterable<C>::value &
            !is_string<C>::value> >
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& out, const C& c)
{
  // using Elt = typename std::iterator_traits<decltype(std::begin(c))>::value_type;
  print_iterable(out, c);
  return out;
}

template <class CharT, class Traits, class Variadic, size_t... Is>
void
_print_variadic(std::basic_ostream<CharT, Traits>& out, const Variadic& t, mp::indices<Is...>)
{
  out << '(';
  swallow {(
      out << (Is == 0 ? "" : ", ") << std::get<Is>(t),
      0)...};
  out << ')';
}

template <class CharT, class Traits, class... Ts>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& out, const std::tuple<Ts...>& t)
{
  _print_variadic(out, t, util::mp::indices_of<std::tuple<Ts...> > {});
  return out;
}

CREATE_MEMBER_FUNCTION_TEST(to_string);
template <class T>
using has_to_string = detect_mem_fn_to_string<
  typename std::decay<T>::type, std::string>;

template <class T,
          class = std::enable_if_t<has_to_string<T>::value> >
std::string to_string(T&& a)
{ return a.to_string(); }


template <class T, class CharT, class Traits,
          class = std::enable_if_t<has_to_string<T>::value> >
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& out, T&& a)
{ return out << to_string(a); }




template <class CharT, class Traits, class... Args>
std::basic_ostream<CharT, Traits>&
print_to(std::basic_ostream<CharT, Traits>& out, Args&&... args)
{
  swallow {(out << std::forward<Args>(args), 0)...};
  return out;
}

// TODO convert encoding

// template <class CharT, class Traits, class... Args>
// std::basic_ostream<CharT, Traits>&
// print(Args&&... args)
// { return print_to(std::cout, std::forward<Args>(args)...); }

// template <class CharT, class Traits, class... Args>
// std::basic_ostream<CharT, Traits>&
// println(Args&&... args)
// { return print(std::forward<Args>(args)..., '\n'); }

template <class... Args>
std::ostream&
print(Args&&... args)
{ return print_to(std::cout, std::forward<Args>(args)...); }

template <class... Args>
std::ostream&
println(Args&&... args)
{ return print(std::forward<Args>(args)..., '\n'); }

template <class CharT, class Traits, class... Args>
std::basic_string<CharT, Traits>
text(Args&&... args)
{
  std::basic_stringstream<CharT, Traits> buf;
  print_to(buf, std::forward<Args>(args)...);
  return buf.str();
}

}

using namespace io;

}

#endif

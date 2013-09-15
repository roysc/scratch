#include <sstream>
#include <iostream>
#include <string>

#include "util/common.hh"
#include "util/traits.hh"
#include "util/mp.hh"

#ifndef _SCRATCH_UTIL_IO
#define _SCRATCH_UTIL_IO

namespace util
{
    namespace io
    {
        template <class... Args>
        std::ostream& print_to(std::ostream& out, Args&&... args)
        {
            swallow {(out << std::forward<Args>(args), 0)...};
            return out;
        }

        template <class... Args>
        std::ostream& print(Args&&... args)
        { return print_to(std::cout, std::forward<Args>(args)...); }

        template <class... Args>
        std::ostream& println(Args&&... args)
        { return print(std::forward<Args>(args)..., '\n'); }

        
        template <class Tuple, size_t... Is>
        void _print_tuple(std::ostream& out, const Tuple& t, indices<Is...>)
        {
            out << '(';
            swallow {(
                    out << (Is == 0 ? "" : ", ") << std::get<Is>(t),
                    0)...};
            out << ')';
        }

        template <class... Ts>
        std::ostream& operator<<(std::ostream& out,
                                 const std::tuple<Ts...>& t)
        { _print_tuple(out, t, indices_for<std::tuple<Ts...> > {}); }


        template <class T>
        using is_char = is_member<T, char, wchar_t, char16_t, char32_t>;

        template <class T>
        struct is_string
        {
            template <class T_ = std::decay_t<T>,
                      class Val = typename std::iterator_traits<
                          decltype(std::begin(std::declval<T_>()))
                          >::value_type>
            static std::enable_if_t<
                is_char<Val>::value &&
                std::is_same<T_, std::basic_string<Val> >::value,
                std::true_type> test(int);
            static std::false_type test(...);
            static const bool value = decltype(test(0))::value;
        };

        
        template <class C,
                  class = std::enable_if_t<
                      is_iterable<C>::value &&
                      !is_string<C>::value> >
        std::ostream& operator<<(std::ostream& out, const C& c)
        {
            using Elt = typename std::iterator_traits<
                decltype(std::begin(c))>::value_type;
            
            out << '[';
            bool first = true;
            for (auto it = std::begin(c); it != std::end(c); ++it) {
                if (!first) out << ", ";
                auto quote = is_char<Elt>::value
                    ? "'" : is_string<Elt>::value
                    ? "\"" : "";
                print_to(out, quote, *it, quote);
                first = false;
            }
            out << ']';
        }
        
        CREATE_MEMBER_FUNCTION_TEST(to_string);
        template <class T>
        using has_to_string = detect_mem_fn_to_string<
            typename std::decay<T>::type, std::string>;
        
        template <class T,
                  class = std::enable_if_t<has_to_string<T>::value> >
        std::string to_string(T&& a)
        { return a.to_string(); }
        
        
        template <class T,
                  class = std::enable_if_t<has_to_string<T>::value> >
        std::ostream& operator<<(std::ostream& out, T&& a)
        { return out << to_string(a); }

    }

    using namespace io;

}

#endif

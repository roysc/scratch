#include <array>
#include <list>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <map>

#include <string>

#include "common.hh"
#include "util.hh"

#ifndef _SCRATCH_COMPONENT
#define _SCRATCH_COMPONENT

#ifdef _DEBUG
using namespace util::io;
#endif

struct BasicComponent
{
    static std::string name() { return "BasicComponent"; }
    std::string to_string() const { return name(); }
};

CREATE_MEMBER_FUNCTION_TEST(name);
template <class T>
using has_name = detect_mem_fn_name<T, std::string>;

template <class T>
util::enable_if_t<has_name<T>::value,
                  std::string>
name()
{ return T::name(); }

template <class T>
util::enable_if_t<!has_name<T>::value,
                  std::string>
name()
{ return typeid(T).name(); }


/** Provides unique identifiers for Components;
 *  Determines Component layout in entities.
 */
template <class... Cpts>
using ComponentIndex = util::TypeVector<Cpts...>;


#ifdef _BUILD_TEST

#endif

#endif

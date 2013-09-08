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
    std::string to_string() const
    {
        return std::string("BasicComponent");
    }
};

/** Provides unique identifiers for Components;
 *  Determines Component layout in entities.
 */
template <class... Cpts>
using ComponentIndex = util::TypeVector<Cpts...>;


#ifdef _BUILD_TEST

#endif

#endif

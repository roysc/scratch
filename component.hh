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

// CREATE_MEMBER_TEST(name);

// template <class C>
// util::enable_if_t<!has__name<C>::value,
//                   std::string> name()
// { return typeid(C).name(); }

// template <class C>
// util::enable_if_t<has__name<C>::value,
//                   std::string> name()
// { return C::name(); }


/** Provides unique identifiers for Components;
 *  Determines Component layout in entities.
 */
template <class... Cpts>
using ComponentIndex = util::TypeVector<Cpts...>;


// template <class... Cpts>
// struct ComponentIndex
//     : public util::TypeVector<Cpts...>
//     // : public util::TypeVector<typename std::add_pointer<Cpts>::type...>
// {
//     // using Components = util::TypeVector<Cpts...>;

    
//     /* Type predicates */
//     /** Whether this ComponentIndex supports a given Component */
//     template <class> struct supports;
    
//     template <class Cpt>
//     struct supports
//         : public util::is_member<Cpt, Cpts...> { };

//     template <template <class...> class CIx,
//               class... OtherCpts>
//     // others must form subset
//     struct supports<CIx<OtherCpts...>>
//         : public util::all_satisfy<supports, OtherCpts...> { };
           
// };


#ifdef _BUILD_TEST

#endif

#endif

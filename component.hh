#include <array>
#include <list>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <map>

#include <iostream>

#include "util.hh"


#ifndef _SCRATCH_COMPONENT_
#define _SCRATCH_COMPONENT_

using EntityID = ulong;

DECLARE_SFINAE_TEST(Dependencies);

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

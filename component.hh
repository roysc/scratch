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

template <class Component>
struct Dependencies
{
    using Type = util::TypeVector<>;
};

template <class Component>
using DependenciesOf = typename Dependencies<Component>::Type;

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


/** Represents logic involving several component subsystems 
 */
template <class... Cpts>
struct LogicSystem
{
    // struct Operation;
    // using OperationQueue = typename std::list<Operation>;

    // OperationQueue op_queue;
    // EntitySpace space;
    
    void update() {}
};


#ifdef _BUILD_TEST

// struct C0 : Component {};
// struct C1 : Component {};

// ComponentIndex<C0, C1> cix;
// static_assert(, "");

#endif

#endif

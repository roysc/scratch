#include <array>
#include <list>
#include <type_traits>
#include <tuple>

#include "util.hh"

#ifndef _SCRATCH_COMPONENT
#define _SCRATCH_COMPONENT

struct Component
{
    using Ident = uint;
    static const Ident ID = 0;
};

/** The Component trait, or contract.
 *      . must inherit from Component
 *      X must have a ::Facade struct // instead, select those needing one?
 *      . ?
 */
template <class C>
struct has_Facade : std::integral_constant<
    decltype(typename C::Facade(), bool()),
    true>
{};

template <class C>
struct is_Component
    : std::is_base_of<Component, C>
    // , std::enable_if<has_Facade<C>::value>
{};

/** Provides unique identifiers for Components;
 *  Determines Component layout in entities.
 */
template <class... Cpts>
struct ComponentIndex
    : std::enable_if<util::all_satisfy<is_Component, Cpts...>::value>
{
    template <class C> struct _facade_ptr
    { using type = typename std::add_pointer<typename C::Facade>; };
    
    using Components = typename
        util::TypeVector<std::add_pointer<Cpts>...>;
    using Facades    = typename
        util::transform<_facade_ptr, typename
                        util::filter<has_Facade,
                                     util::TypeVector<Cpts...>
                                     >::type>::type;
    
    /* Type predicates */
    /** Whether this ComponentIndex supports a given Component */
    template <class C>
    struct has_component : util::is_member<C, Cpts...> { };
    /** Whether an Entity implementing these Components can be implemented */
    template <class... Cs>
    struct is_valid_entity : util::all_satisfy<has_component, Cs...> { };
           
};

#ifdef _BUILD_TEST

struct C : Component { struct Facade {}; };

void test() {
    ComponentIndex<C> cix;
    // static_assert(, "");
}
#endif

#endif

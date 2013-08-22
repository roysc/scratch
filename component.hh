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
 *      . must have a ::Facade struct
 *      . ?
 */
template <class C>
struct has_Facade
{
    static constexpr auto _has_Facade()
        -> decltype(typename C::Facade(), bool()) {
        return true;
    }
    static constexpr bool value = _has_Facade();
};
template <class C>
struct is_Component
    : std::is_base_of<Component, C>
    , std::enable_if<has_Facade<C>::value>
{};


/** Provides unique identifiers for Components;
 *  Determines Component layout in entities.
 */
template <class... Cpts>
struct ComponentIndex
    : std::enable_if<util::all_satisfy<is_Component, Cpts...>::value>
{
    // struct ComponentVector
    //     : util::TypeVector<std::add_pointer<Cpts>...>
    // {};
    // struct Facades
    //     : util::TypeVector<std::add_pointer<typename Cpts::Facade>...>
    // {};
    
    // template <class C> struct _facade_ptr
    // { using type = typename std::add_pointer<typename C::Facade>; }
    
    using Components = typename
        util::TypeVector<std::add_pointer<Cpts>...>;
    using Facades    = typename // no way to filter for Facade-defn's?
        // util::filter_map<has_Facade, _facade_ptr, util::TypeVector, Cpts...>;
        util::TypeVector<std::add_pointer<typename Cpts::Facade>...>;

    
    /** Type-checking helpers */
    template <class C>
    struct has_component : util::is_member<C, Cpts...> { };
    /** Whether an entity implementing these components can be implemented */
    template <class... Cs>
    struct is_valid_entity : util::all_satisfy<has_component, Cs...> { };
           
};

#endif

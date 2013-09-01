#include <array>
#include <list>
#include <type_traits>
#include <tuple>

#include "util.hh"

#ifndef _SCRATCH_COMPONENT_
#define _SCRATCH_COMPONENT_

struct Component
{
    using Ident = uint;
    static const Ident ID = 0;

    using InputType = void;
};

template <class Cpt>
struct Subsystem
{
    static const size_t init_capacity = 100;
    std::vector<Cpt> data;
    
    Subsystem()
        : data(init_capacity)
    {}

    void update() {}

    Cpt* create() {
        // assert(false, "Must implement specialized create() method");

        auto ctor_input = input.next<Cpt::InitData>();
        Cpt cpt(ctor_input);
        
    }
};

/** The Component trait contract:
 *  - must inherit from Component
 */
template <class Cpt>
struct is_Component
    : public std::is_base_of<Component, Cpt>
    // , std::enable_if<has_Subsystem<Cpt>::value>
{};

/** Provides unique identifiers for Components;
 *  Determines Component layout in entities.
 */
template <class... Cpts>
struct ComponentIndex
    : public std::enable_if<util::all_satisfy<is_Component, Cpts...>::value>
{
    // template <class Cpt> struct _subsystem_ptr
    // { using type = typename std::add_pointer<typename Cpt::Subsystem>; };

    /// A set of pointers to component objects.
    /// Used in entity objects.
    using Components = typename util::TypeVector<std::add_pointer<Cpts>...>;

    /// A set of subsystems corresponding to component types
    using Subsystems = typename util::TypeVector<Subsystem<Cpts>...>;

    // std::array<Subsystem*, sizeof...(Cpts)>;   
    
    /* Type predicates */
    /** Whether this ComponentIndex supports a given Component */
    template <class Cpt>
    struct has_component : util::is_member<Cpt, Cpts...> { };
    /** Whether an Entity implementing these Components can be implemented */
    template <class... Cs>
    struct is_valid_entity : util::all_satisfy<has_component, Cs...> { };
           
};

#ifdef _BUILD_TEST

struct C0 : Component {};
struct C1 : Component {};

ComponentIndex<C0, C1> cix;
// static_assert(, "");

#endif

#endif

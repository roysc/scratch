#include <array>
#include <list>
#include <type_traits>
// #include <tuple>
#include <unordered_map>

#include "util.hh"

#ifndef _SCRATCH_COMPONENT_
#define _SCRATCH_COMPONENT_

using EntityID = ulong;

struct Component
{
    // using Ident = uint;
    // static const Ident ID = 0;

    using InputType = void;
};

template <class Cpt>
struct Subsystem
{
    // static const size_t init_capacity = 100;
    
    using Data = std::unordered_map<EntityID, Cpt>;
    // std::vector<Cpt>;
    Data data;
    

    void update() {}

    template <class Source>
    Cpt* create(EntityID ent_id, Source& src) {
        // assert(false && "Must implement specialized create() method");

        // auto input = src.next<typename Cpt::InputType>();
        // data[ent_id] = Cpt(input);
        // return &data[ent_id];
    }
};


template <class Cpt>
struct Dependencies
{
    
};

/** Provides unique identifiers for Components;
 *  Determines Component layout in entities.
 */
template <class... Cpts>
struct ComponentIndex
    : public util::TypeVector<std::add_pointer<Cpts>...>
{
    // using Components = typename util::TypeVector<std::add_pointer<Cpts>...>;
    // Components contents;

    
    
    /* Type predicates */
    /** Whether this ComponentIndex supports a given Component */
    template <class> struct supports;
    
    template <class Cpt>
    struct supports
        : public util::is_member<Cpt, Cpts...> { };

    template <template <class...> class CIx,
              class... OtherCpts>
    // others must form subset
    struct supports<CIx<OtherCpts...>>
        : public util::all_satisfy<supports, OtherCpts...> { };
           
};

#ifdef _BUILD_TEST

// struct C0 : Component {};
// struct C1 : Component {};

// ComponentIndex<C0, C1> cix;
// static_assert(, "");

#endif

#endif

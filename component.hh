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

// Manages a single type of component
template <class Cpt>
struct Subsystem
{
    // static const size_t init_capacity = 100;
    
    using Data = std::unordered_map<EntityID, Cpt>;
    Data data;
    
    template <class Source>
    Cpt* create(EntityID ent_id, Source& src) {
    // Cpt* create(EntityID ent_id) {
        // assert(false && "Must implement specialized create() method");

        // using InputType = typename Cpt::InputType;

        // InputType input = src.template next<InputType>();
        data.emplace(ent_id, Cpt());

        return &data[ent_id];
    }
};


/** Provides unique identifiers for Components;
 *  Determines Component layout in entities.
 */
template <class... Cpts>
struct ComponentIndex
    : public util::TypeVector<typename std::add_pointer<Cpts>::type...>
{
    using Components = util::TypeVector<Cpts...>;

    
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

template <class LogicSystem>
struct Dependencies : ComponentIndex<>
{ };

/** Represents logic involving several component subsystems 
 */
template <class SystemIndex>
          // class ComponentSpace>
struct LogicSystem
{
    // struct Operation;
    // using OperationQueue = typename std::list<Operation>;

    // OperationQueue op_queue;
    // ComponentSpace space;
    
    
    void update() {}

    
};

// template <class... LSs>
// struct LogicIndex


#ifdef _BUILD_TEST

// struct C0 : Component {};
// struct C1 : Component {};

// ComponentIndex<C0, C1> cix;
// static_assert(, "");

#endif

#endif

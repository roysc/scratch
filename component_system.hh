
#include "component.hh"

#ifndef _SCRATCH_COMPONENT_SYSTEM_
#define _SCRATCH_COMPONENT_SYSTEM_


template <class CIx>
/** Anything that "exists" within the system */
struct Entity
{
    using CIx::Components::get;
    // using CIx::Components::set;
    
};


/**
 * Represents a system which can handle the specified components.
 * 
 */
template <class CIx>
struct ComponentSystem
{
    /** Anything that "happens" within the system */
    struct Operation;


    /**** Methods ****
     */
    ComponentSystem()
        // : facades
    {
        
    }

    void update() {
        for (auto& subsystem : subsystems) {
            facade.update();
        }
    }
    
    void enqueue(Operation&& op) {}

    // template <class... Cs>
    EntityID create() {
        EntityID = fresh_ident();
        
        Entity e;
    }


    
    /**** "Interface" objects ****
     * represent operations or tools...
     */
    // template <class...> struct Spawn;


    /**** Operation ****
     * performed by component
     * on entities implementing component */
    struct Operation
    {
        virtual void execute();
    };

    using OperationQueue = typename std::list<Operation>;


    /**** Entity ****
     *  entity is a simple aggregation of components */
    struct Entity : public CIx::Components
    {
        using Ident = int;
        static const Ident max_identifiers = 1000;

    };

    using EntityID = typename Entity::Ident;
    
    using Entities = typename
        std::array<Entity<CIx>, Entity::max_identifiers>;
    
    // static EntityID fresh_ident() {
    //     static EntityID current_ident = 0;
        
        
    // }

    

    
    
    typename CIx::Facades facades;
    Entities              entities;
    OperationQueue        op_queue;
};


// /** Create entity */
// // does this need to be aware of a specific system?
// template <class CIx>
// template <class... Cs>
// struct ComponentSystem<CIx>::Spawn
//     : Operation
//     , std::enable_if<CIx::template is_valid_entity<Cs...>::value>
// {
//     void execute(ComponentSystem& system) {
//         // std::unique_ptr<Entity>&
//         Entity::Ident ent = system.entities.fresh_ident();

//         // for C in Cs...
//         // system.facades.get<C>().initiate();
        
//     }
// };



#ifdef _BUILD_TEST

struct C0 : Component {};
struct C1 : Component {};

ComponentSystem<ComponentIndex<C0, C1> > csys;
// static_assert(, "");

#endif

#endif

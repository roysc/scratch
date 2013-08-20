// #include <type_traits>
#include "component.hh"

#ifndef _SCRATCH_COMPONENT_SYSTEM
#define _SCRATCH_COMPONENT_SYSTEM

/**
 * Represents a system which can handle the specified components.
 * 
 */
template <class CIx>
struct ComponentSystem
{
    /** Anything that "exists" within the system */
    struct Entity;
    /** Anything that "happens" within the system */
    struct Operation;
    
    // using CSys = ComponentSystem<CIx>;
    using OperationQueue = typename
        std::list<Operation>;
    using Entities       = typename
        std::array<Entity, Entity::max_identifiers>;

    /** "Interface" objects, representing operation or other tool */
    template <class...> struct Spawn;
    // template <class...> struct ;



    // struct Time : Component {};
    // using Components = typename CIx::ComponentVector;
    
    struct Entity : CIx::Components
    {
        using Ident = int;
        static const Ident max_identifiers = 1000;
    };

    // What is an operation?
    //    * performed by component
    //    * on entities with component
    struct Operation
    { virtual void execute(); };

    ComponentSystem() {}
    void enqueue(Operation op) {}

    // template <class... Cs> Entity::Ident create();
    // Entities& entities();

  private:
    typename CIx::Facades facades;

    Entities _entities;
    CIx _index;
    OperationQueue _op_queue;

  public:
    using EntityID = typename Entity::Ident;

    /** Create entity */
    // does this even need to be aware of a specific system?
    template <class... Cs>
    struct Spawn
        : Operation
        , std::enable_if<CIx::template is_valid_entity<Cs...>::value>
    {
        
        void execute(ComponentSystem& system) {
            // std::unique_ptr<Entity>&
            EntityID ent = system.entities.fresh_ident();

            // for C in Cs...
            // system.facades.get<C>().initiate();
        
        }
    };

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

#endif

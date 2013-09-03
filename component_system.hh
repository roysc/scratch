// #include 

#include "component.hh"
#include "util.hh"

#ifndef _SCRATCH_COMPONENT_SYSTEM_
#define _SCRATCH_COMPONENT_SYSTEM_



/**
 * Represents a system which can handle the specified components.
 * 
 */
template <class SystemIndex>
struct ComponentSystem
{
    using Subsystems = typename util::TypeVector<Subsystem<Cpts>...>;

    struct Operation;
    using OperationQueue = typename std::list<Operation>;


    template <class> struct Entity;
    // using Entities = 


    // members
    Subsystems     subsystems ;
    Entities       entities   ;
    OperationQueue op_queue   ;

    
    void update()
    {
        for (auto& subsystem : subsystems) {
            facade.update();
        }
    }
    
    void enqueue(Operation&& op)
    {
        
    }

    template <class EntityIndex, class Source>
    Entity<EntityIndex> create_entity(Source& src)
    {
        Entity<EntityIndex> entity;

        
        auto cpt = subsystems.get<Cpt>().create(src);
        entity.add_component<Cpt>(cpt);
        
        
        // template <class Subsystems, class Ent>
        // struct Initialize
        // {
        //     Subsystems& subsystems;
        //     Ent& entity;
            
        //     template <class Cpt>
        //     void operator()(Cpt* cpt)
        //     {
        //         auto cpt = subsystems.get<Cpt>().create(src);
        //         entity.add_component<Cpt>(cpt);
        //     }
        // }
        
        // boost::mpl::for_each<EntityIndex::Components>(
        //     Initialize{subsystems, entity});
    }


    /**** Operation ****
     * performed by component
     * on entities implementing component */
    struct Operation
    {
        virtual void execute();
    };


    /**** Entity ****
     *  Anything that "exists" within the system 
     *  Contains a simple aggregation of components
     */
    template <class EntityIndex>
    struct Entity
    {
        EntityIndex index;

        template <class Cpt>
        void add(Cpt* cpt)
        {
            Dependencies<Cpt>::Types
            
            std::vector<size_t> ixs;
            for (auto& ix : ixs)
                assert(index.get<Cpt>() != nullptr);
            
            index.get<Cpt>() = cpt;
            // import std.stdio; writeln(*index.get!Cpt);
        }

        
        
        using SystemIndex::Components::get;
        // using SystemIndex::Components::set;
    
    };
};


// /** Create entity */
// // does this need to be aware of a specific system?
// template <class SystemIndex>
// template <class... Cs>
// struct ComponentSystem<SystemIndex>::Spawn
//     : Operation
//     , std::enable_if<SystemIndex::template is_valid_entity<Cs...>::value>
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

#include <boost/mpl/for_each.hpp>

#include "component.hh"
#include "util.hh"

#ifndef _SCRATCH_COMPONENT_SYSTEM_
#define _SCRATCH_COMPONENT_SYSTEM_

namespace functor
{
    template <class ComponentSystem, class Entity, class Source>
    struct InitComponent
    {
        ComponentSystem& self;
        Entity& entity;
        Source& src;
        
        template <class Cpt>
        void operator()(Cpt _)
        {
            auto sub = self.subsystems.template get<Cpt>();
            entity.add_component(sub.create(src));
        }
    };
        
    template <class Entity>
    struct VerifyComponent
    {
        Entity& self;
        
        template <class Cpt>
        void operator()(Cpt _)
        {
            auto cpt_ptr = self.index.template get<Cpt>();
            assert(cpt_ptr != nullptr &&
                   "Component dependency is not initialized!\n");
        }
    };

    template <class ComponentSystem>
    struct UpdateSubsystem
    {
        ComponentSystem& self;

        template <class Subsystem>
        void operator()(Subsystem _)
        {
            self.subsystems.template get<Subsystem>().update();
        }
    };

}

/**
 * Represents a system which can handle the specified components.
 * 
 */
template <class SystemIndex>
struct ComponentSystem
{
    using Subsystems = typename util::transform<Subsystem, SystemIndex>;

    struct Operation;
    using OperationQueue = typename std::list<Operation>;


    template <class> struct Entity;
    // using Entities = std::vector


    // members
    Subsystems     subsystems ;
    // Entities       entities   ;
    OperationQueue op_queue   ;

    
    void update()
    {
        functor::UpdateSubsystem<
            decltype(*this)
            > f{*this};

        boost::mpl::for_each<Subsystems>(f);
        // // with C++14:
        // boost::mpl::for_each<Subsystems>(
        //     [&] <class Subsystem> (Subsystem _) {
        //         subsystems.template get<Subsystem>().update();
        //     });
    }
    
    void enqueue(Operation&& op) { }
    
    template <class EntityIndex, class Source>
    Entity<EntityIndex>& create_entity(Source& src)
    {
        Entity<EntityIndex> entity;

        functor::InitComponent<
            decltype(*this),
            Entity<EntityIndex>,
            Source
            > f{*this, entity, src};
        
        boost::mpl::for_each<EntityIndex>(f);
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
        void add_component(Cpt* cpt)
        {
            functor::VerifyComponent<
                decltype(*this)
                > f{*this};

            boost::mpl::for_each<Dependencies<Cpt> >(f);
            
            index.template get<Cpt>() = cpt;
        }
        
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

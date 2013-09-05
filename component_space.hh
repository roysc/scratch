#include <vector>
#include <memory>
#include <type_traits>

#include "util.hh"
#include "component.hh"
#include "entity.hh"

#ifndef _SCRATCH_COMPONENT_SPACE_
#define _SCRATCH_COMPONENT_SPACE_


// Manages a single type of component
template <class Component>
struct Subsystem
{
    using Reference = Ref<Component>;
    using Data = std::unordered_map<EntityID, Reference>;
    Data data;

    Subsystem()
    {
        data.reserve(100);
    };


    Reference create(EntityID ent_id) {
        // assert(false && "Must implement specialized create() method");

        // std::cout << "creating " << typeid(Component).name()
        //           << " with ID = " << ent_id
        //           << " at " << ref << "\n";

        // data.emplace(ent_id, Component());
        // return Reference {&data[ent_id]};
        
        Reference ref =
            data[ent_id] = Reference {new Component()};

        return ref;
    }
};

/**
 * An environment which manages the specified components.
 * 
 */
template <class... Components>
struct ComponentSpace
{
    // using Subsystems = util::TypeVector<Subsystem<Components>...>;
    using Index = util::TypeVector<Components...>;
    using EntityType = Entity<Components...>;
    using Entities = std::vector<EntityType>;
    
    // members
    // Subsystems m_subsystems;
    Entities m_entities;
    

    // struct UpdateSubsystem
    // {
    //     ComponentSpace& self;

    //     template <class Subsystem>
    //     void operator()(Subsystem)
    //     { self.template get_subsystem<Subsystem>().update(); }
    // };
    
    void update()
    {
        // util::expand_apply<Subsystems>(
        //     functor::make<UpdateSubsystem>(*this)
        // );
    }
    
    
    template <class... InitCpts>
    EntityID create_entity()
    {
        EntityType entity (
            Ref<InitCpts> {new InitCpts()}...
            // this->template get_subsystem<InitCpts>().create(id)... 
        );

        EntityID id = 0;
        for (; id < m_entities.size(); ++id) {
            if (m_entities[id].is_empty()) {
                m_entities[id] = entity;
                return id;
            }
        }
        
        m_entities.push_back(entity);
        return id;
    }

    EntityType& operator[](EntityID id)
    {
        return m_entities[id];
    }
    
    // template <class Cpt>
    // Subsystem<Cpt>& get_subsystem()
    // { return util::get<Subsystem<Cpt> >(m_subsystems); }
    

    /**** Operation ****
     * performed by component
     * on entities implementing component */
    struct Operation
    {
        virtual void execute();
    };

};

namespace functor
{
    template <class ComponentSpace, class Entity>
    struct InitComponent
    {
        ComponentSpace self;
        Entity& entity;
        EntityID id;
        
        template <class Cpt>
        void operator()(Cpt _)
        {
            Subsystem<Cpt> sub = self->template get_subsystem<Cpt>();
            entity.add_component(sub.create(id));
        }
    };
}


// /** Create entity */
// // does this need to be aware of a specific system?
// template <class FullIndex>
// template <class... Cs>
// struct ComponentSpace<FullIndex>::Spawn
//     : Operation
//     , std::enable_if<FullIndex::template is_valid_entity<Cs...>::value>
// {
//     void execute(ComponentSpace& system) {
//         // std::unique_ptr<Entity>&
//         Entity::Ident ent = system.entities.fresh_ident();

//         // for C in Cs...
//         // system.facades.get<C>().initiate();
        
//     }
// };



#ifdef _BUILD_TEST

struct C0 : Component {};
struct C1 : Component {};

ComponentSpace<ComponentIndex<C0, C1> > csys;
// static_assert(, "");

#endif

#endif

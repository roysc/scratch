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

        // data.emplace(ent_id, Component());
        Reference ref =
            data[ent_id] = Reference {new Component()};
        
        std::cout << "creating " << typeid(Component).name()
                  << " with ID = " << ent_id
                  << " at " << ref << "\n";
        
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
    using Subsystems = typename
        util::transform_t<Subsystem, util::TypeVector<Components...> >;

    
    // members
    Subsystems m_subsystems;
    // Entities entities;
    std::vector<bool> m_used_ids;

    
    void update()
    {
        functor::UpdateSubsystem<decltype(this)> f {this};
        util::expand_apply<Subsystems>(f);
    }
    
    // template <class EntityIndex>
    // Entity<EntityIndex>& create_entity()
    template <class... EntityCpts>
    // Entity<ComponentIndex<EntityCpts...> >
    auto create_entity()
    {
        using EntityType = Entity<ComponentIndex<EntityCpts...> >;
        // EntityType entity;

        EntityID id = fresh_id();

        // std::tuple<typename std::add_pointer<EntityCpts>::type...> t {
        //     this->template get_subsystem<EntityCpts>().create(id)...
        // };
        
        EntityType entity (
            this->template get_subsystem<EntityCpts>().create(id)... 
        );
        
        // functor::InitComponent<decltype(this), Ent>
        //     f {this, entity, id};
        // util::expand_apply<EntityIndex>(f);

        return entity;
    }
    
    EntityID fresh_id()
    {
        EntityID id = 0;
        for (; id < m_used_ids.size(); ++id) {
            if (!m_used_ids[id]) {
                m_used_ids[id] = true;
                return id;
            }
        }
        m_used_ids.push_back(true);
        return id;
    }

    template <class Cpt>
    Subsystem<Cpt>& get_subsystem()
    { return util::get<Subsystem<Cpt> >(m_subsystems); }
    

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

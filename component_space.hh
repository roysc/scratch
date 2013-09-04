#include <utility>

#include "util.hh"
#include "component.hh"
#include "entity.hh"

#ifndef _SCRATCH_COMPONENT_SPACE_
#define _SCRATCH_COMPONENT_SPACE_


// Manages a single type of component
template <class Cpt>
struct Subsystem
{
    using Data = std::unordered_map<EntityID, Cpt>;
    Data data;

    Subsystem()
    {
        data.reserve(100);
    };
    
    Cpt* create(EntityID ent_id) {
        // assert(false && "Must implement specialized create() method");

        // data.emplace(ent_id, Cpt());
        data[ent_id] = Cpt();
        auto loc = &data[ent_id];
        
        std::cout << "creating " << typeid(Cpt).name()
                  << " with ID = " << ent_id
                  << " at " << loc << "\n";
        
        return loc;
    }
};

/**
 * An environment which manages the specified components.
 * 
 */
template <class... Cpts>
struct ComponentSpace
{
    using Subsystems = typename
        util::transform_t<Subsystem, util::TypeVector<Cpts...> >;

    // using Entities = std::vector


    // members
    Subsystems m_subsystems;
    // Entities entities;
    std::vector<bool> m_used_ids;

    
    void update()
    {
        using namespace functor;

        UpdateSubsystem<decltype(this)> f {this};
        for_each<Subsystems>(f);
    }
    
    template <class EntityIndex>
    Entity<EntityIndex>& create_entity()
    {
        using Ent = Entity<EntityIndex>;
        Ent entity;
        EntityID id = fresh_id();

        using namespace functor;

        InitComponent<decltype(this), Ent> f {this, entity, id};
        for_each<EntityIndex>(f);

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
    {
        return util::get<Subsystem<Cpt> >(m_subsystems);
    }
    

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

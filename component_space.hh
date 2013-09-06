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
        //     util::make<UpdateSubsystem>(*this)
        // );
    }
    
    
    template <class... InitCpts>
    EntityID create_entity()
    {
        auto it = m_entities.begin();
        for (; it != m_entities.end(); ++it) 
            if (it->is_empty())
                break;
        
        m_entities.insert(
            it, EntityType(Ref<InitCpts> {new InitCpts()}...)
        );

        EntityID id = it - m_entities.begin();
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


#ifdef _BUILD_TEST

struct C0 : Component {};
struct C1 : Component {};

ComponentSpace<ComponentIndex<C0, C1> > csys;
// static_assert(, "");

#endif

#endif

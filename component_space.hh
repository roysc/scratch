#include <utility>

#include "component.hh"
#include "util.hh"

#include <boost/mpl/for_each.hpp>


#ifndef _SCRATCH_COMPONENT_SPACE_
#define _SCRATCH_COMPONENT_SPACE_


namespace functor
{
    using boost::mpl::for_each;
    
    template <class ComponentSpace,
              class Entity> struct InitComponent;
    template <class Entity> struct VerifyComponent;
    template <class ComponentSpace> struct UpdateSubsystem;
}

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
        using namespace functor;

        VerifyComponent<decltype(this)> f {this};
        for_each<Dependencies<Cpt> >(f);
            
        util::get<Cpt*>(index) = cpt;
    }

    template <class Cpt>
    Cpt* get_component()
    {
        return util::get<Cpt*>(index);
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
    Subsystems subsystems;
    // Entities entities;
    std::vector<bool> used_ids;

    
    void update()
    {
        using namespace functor;

        UpdateSubsystem<decltype(this)> f {this};
        for_each<Subsystems>(f);
    }
    
    template <class EntityIndex>
    Entity<EntityIndex>& create_entity()
    {
        Entity<EntityIndex> entity;
        EntityID id = fresh_id();

        using namespace functor;

        InitComponent<decltype(this),
                      EntityIndex> f {this, entity, id};
        for_each<typename EntityIndex::Components>(f);

        return entity;
    }
    
    EntityID fresh_id()
    {
        EntityID id = 0;
        for (; id < used_ids.size(); ++id) {
            if (!used_ids[id]) {
                used_ids[id] = true;
                return id;
            }
        }
        used_ids.push_back(true);
        return id;
    }

    template <class Cpt>
    Subsystem<Cpt>& get_subsystem()
    {
        return util::get<Subsystem<Cpt> >(subsystems);
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
    // By C++14, this can be replaced with e.g.:
    // for_each<EntityIndex>(
    //     [&] <class Cpt> (Cpt _) {
    //         auto sub = self.subsystems.template get<Cpt>();
    //         entity.add_component(sub.create(src));
    // ...
    //     [&] <class Subsystem> (Subsystem _) {
    //         subsystems.template get<Subsystem>().update();
    //     });

    // // does not work for constructors with diff. arity than tpl. params
    // template <template <class...> class Functor, class... Args>
    // Functor<Args...> make(Args... args) {
    //     return Functor<Args...> {std::forward(args)...};
    // }
    
    template <class ComponentSpace, class EntityIndex>
    struct InitComponent
    {
        ComponentSpace self;
        Entity<EntityIndex>& entity;
        EntityID id;
        
        template <class Cpt>
        void operator()(Cpt _)
        {
            Subsystem<Cpt> sub = self->template get_subsystem<Cpt>();
            entity.add_component(sub.create(id));
        }
    };

    template <class Entity>
    struct VerifyComponent
    {
        Entity self;
        
        template <class Cpt>
        void operator()(Cpt _)
        {
            auto cpt_ptr = util::get<Cpt*>(self->index);
            assert(cpt_ptr != nullptr &&
                   "Component dependency is not initialized!\n");
        }
    };

    template <class ComponentSpace>
    struct UpdateSubsystem
    {
        ComponentSpace self;

        template <class Subsystem>
        void operator()(Subsystem _)
        {
            util::get<Subsystem>(self->subsystems).update();
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

#include <vector>
#include <memory>
#include <type_traits>

#include "util.hh"
#include "component.hh"
#include "entity.hh"

#ifndef _SCRATCH_ENTITY_SPACE_
#define _SCRATCH_ENTITY_SPACE_

/**
 * An environment which manages the specified components.
 * 
 */
template <class... Components>
struct EntitySpace
{
    using Index = util::TypeVector<Components...>;
    using EntityType = Entity<Components...>;
    using Entities = std::vector<EntityType>;
    
    // members
    Entities m_entities;
    
    void update()
    {
        
    }
    
    template <class... InitCpts>
    EntityID create_entity()
    {
        EntityType entity(Ref<InitCpts> {new InitCpts}...);
        EntityID id = 0;

        auto it = m_entities.begin();
        for (; it != m_entities.end() && !it->is_empty(); ++it, ++id) 
            ;
            // if (it->is_empty()) break;
        

        std::cout << "it = " << it - m_entities.begin() << "\n";

        m_entities.insert(it, std::move(entity));
        
        std::cout << "creating Entity (id = "<<id<<"): ";
        std::cout.flush() << *it << "\n";
        return id;
    }

    EntityType& operator[](EntityID id)
    {
        return m_entities[id];
    }

    
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

EntitySpace<ComponentIndex<C0, C1> > csys;
// static_assert(, "");

#endif

#endif

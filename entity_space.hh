#include <vector>
#include <memory>

#include "util.hh"
#include "entity.hh"

#ifndef _SCRATCH_ENTITY_SPACE
#define _SCRATCH_ENTITY_SPACE

#ifdef _DEBUG
using namespace util::io; // when not debugging, shouldn't have print stmts
#endif

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
    
    template <class... InitCpts>
    EntityID create_entity()
    {
        EntityType entity(Ref<InitCpts> {new InitCpts}...);
        EntityID id = 0;

        auto it = m_entities.begin();
        while (it != m_entities.end() &&
               !it->is_empty())
            ++it, ++id;

        // println("it - begin = ", it - m_entities.begin(), "\n");
        // println("creating Entity (id = ", id, "): ");

        m_entities.insert(it, std::move(entity));
        
        // println("# of entities = ", m_entities.size());
        // println(m_entities[id]);
        
        return id;
    }

    EntityType& operator[](EntityID id)
    { return m_entities[id]; }

};

template <class EntitySpace, class... Components>
auto create_entity =
    std::mem_fn(&EntitySpace::template create_entity<Components...>);


#ifdef _BUILD_TEST

#endif

#endif

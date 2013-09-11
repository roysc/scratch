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
    using Iterator = typename Entities::iterator;

    static const size_t n_components = sizeof...(Components);
    
    // members
    Entities m_entities;
    
    EntityID insert(EntityType&& entity)
    {
        EntityID id = 0;

        auto it = m_entities.begin();
        while (it != m_entities.end() &&
               !it->is_empty())
            ++it, ++id;

        m_entities.insert(it, std::move(entity));
        
        return id;
    }
    
    EntityType& operator[](EntityID id)
    { return m_entities[id]; }

    Iterator begin() { return m_entities.begin(); }
    Iterator end() { return m_entities.end(); }

};


#ifdef _BUILD_TEST

#endif

#endif

#include <utility>
#include <type_traits>
#include <memory>
#include <bitset>
#include <cassert>

#include "util.hh"
#include "component.hh"

#ifndef _SCRATCH_ENTITY
#define _SCRATCH_ENTITY

#ifdef _DEBUG
using namespace util::io;
#endif

template <class Cpt>
using Ref = typename
    // std::add_lvalue_reference<Cpt>::type;
    // std::add_pointer<Cpt>::type;
    // std::shared_ptr<Cpt>;
    std::unique_ptr<Cpt>;

using EntityID = ulong;

/**** Entity ****
 *  Anything that "exists" within the system 
 *  Contains a simple aggregation of components
 */
template <class... Components>
struct Entity
    // : util::transform_t<Ref, EntityIndex>;
{
    using Contents = util::TypeVector<Ref<Components>...>;
    Contents m_components;

    static const size_t capacity = sizeof...(Components);
    using Description = std::bitset<capacity>;
    Description m_description;

    template <class... Cpts>
    Entity(Ref<Cpts>&&... args)
        // : m_components { std::forward<Cpts>(args)... }
    {
        util::swallow {(
            util::get<Ref<Cpts> >(m_components) =
                std::forward<Ref<Cpts> >(args),
            // println("setting Component ", *args),
            
        0)...};

        util::swallow {(
            m_description.set(util::index_of<Cpts, Components...>::value),
        0)...};

        // println(m_description);
    }

    bool is_empty() { return m_description.none(); }
    

    template <class Cpt>
    void add_component(Ref<Cpt>&& cpt)
    {
        util::get<Ref<Cpt> >(m_components) = std::move(cpt);
        m_description.set(util::index_of<Cpt, Components...>::value);
    }

    template <class Cpt>
    bool has_component()
    {
        const auto ix = util::index_of<Cpt, Components...>::value;
        return m_description.test(ix);
        
        // return bool(util::get<Ref<Cpt> >(m_components));
    }
    
    template <class Cpt>
    Cpt get_component()
    {
        // assert(has_component<Cpt>() && "Component is not initialized!\n");
        return *util::get<Ref<Cpt> >(m_components);
    }

    bool supports(std::bitset<capacity> flags)
    {
        util::swallow {(
                m_description.set(
                    util::index_of<Components, Components...>::value,
                    has_component<Components>()),
            0)...};            
        // println()
        
        return (flags & m_description) == m_description;
    }
    
    std::string to_string()
    {
        using util::io::print_to;
        
        std::stringstream out;
        bool at_0 = true;
        
        out << "Entity<";
        util::swallow {(
            has_component<Components>()
            ? (print_to(out, at_0 ? "" : ", ",
                        get_component<Components>())
               , at_0 = false)
            : 0,
        0)...};
        out << ">";
        
        return out.str();
    }

};

#ifdef _DEBUG

#endif

#endif

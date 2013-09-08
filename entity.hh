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

    using Description = std::bitset<sizeof...(Components)>;
    Description m_description;

    template <class... Cpts>
    Entity(Cpts&&... args)
        // : m_components { std::forward<Cpts>(args)... }
    {
        // using ignore = int[sizeof...(Cpts)];
        util::ignore {(
            util::get<Cpts>(m_components) = std::forward<Cpts>(args),
            // println("setting Component"),
            // std::cout << "setting Component: " << *args << "\n",
            
        0)...};

        using CptsVector = util::TypeVector<Cpts...>;
        util::ignore {(
            m_description.set(util::index_within<Cpts, CptsVector>::value),
        0)...};

        std::cout << m_description << "\n";
    }

    bool is_empty() { return m_description.none(); }
    

    template <class Cpt>
    void add_component(Ref<Cpt> cpt)
    {
        util::get<Ref<Cpt> >(m_components) = std::move(cpt);
    }

    template <class Cpt>
    bool has_component()
    {
        return bool(util::get<Ref<Cpt> >(m_components));

        // causes segfault!
        // return m_description[util::index_of<Cpt, Components...>::value];
    }
    
    template <class Cpt>
    Cpt get_component()
    {
        assert(has_component<Cpt>() && "Component is not initialized!\n");
        return *util::get<Ref<Cpt> >(m_components);
    }

    std::string to_string()
    {
        std::stringstream out;
        bool at_0 = true;
        
        out << "Entity<";
        util::ignore {(
            has_component<Components>()
            ? (out
               << (at_0 ? "" : ", ")
               << util::to_string(get_component<Components>())
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

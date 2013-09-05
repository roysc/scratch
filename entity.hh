#include <utility>
#include <type_traits>
#include <memory>

#include "util.hh"
#include "component.hh"

#ifndef _SCRATCH_ENTITY_
#define _SCRATCH_ENTITY_

template <class Cpt>
using Ref = typename
    // std::add_lvalue_reference<Cpt>::type;
    // std::add_pointer<Cpt>::type;
    std::shared_ptr<Cpt>;

/**** Entity ****
 *  Anything that "exists" within the system 
 *  Contains a simple aggregation of components
 */
template <class EntityIndex>
struct Entity
    // : util::transform_t<Ref, EntityIndex>;
{
    using Members =
        util::transform_t<Ref, EntityIndex>;

    Members m_components;

    template <class... Cpts>
    Entity(Cpts&&... args)
        : m_components { std::forward<Cpts>(args)... }
    {}
    
    template <class Cpt>
    void add_component(Ref<Cpt> cpt)
    {
        using namespace functor;

        VerifyComponent<decltype(this)> f {this};
        util::expand_apply<Dependencies<Cpt> >(f);
            
        util::get<Ref<Cpt> >(m_components) = cpt;
    }

    
    template <class Cpt>
    Ref<Cpt> get_component()
    {
        return util::get<Ref<Cpt> >(m_components);
    }
};

#endif

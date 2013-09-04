#include <type_traits>

#include "util.hh"
#include "component.hh"

#ifndef _SCRATCH_ENTITY_
#define _SCRATCH_ENTITY_

template <class Cpt>
using Reference = typename
    std::add_lvalue_reference<Cpt>::type;
    // std::shared_pointer<Cpt>::type;

/**** Entity ****
 *  Anything that "exists" within the system 
 *  Contains a simple aggregation of components
 */
template <class EntityIndex>
struct Entity
{
    using Members =
        // util::transform_t<util::add_pointer_t, EntityIndex>;
        util::transform_t<Reference, EntityIndex>;

    Members m_components;

    template <class Cpt>
    void add_component(Reference<Cpt> cpt)
    {
        using namespace functor;

        VerifyComponent<decltype(this)> f {this};
        for_each<Dependencies<Cpt> >(f);
            
        util::get<Reference<Cpt> >(m_components) = cpt;
    }

    
    template <class Cpt>
    Reference<Cpt> get_component()
    {
        return util::get<Reference<Cpt> >(m_components);
    }
};

#endif

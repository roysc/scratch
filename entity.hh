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
    using Contents = util::transform_t<Ref, EntityIndex>;
    Contents m_components;

    template <class... Cpts>
    Entity(Cpts&&... args)
        // : m_components { std::forward<Cpts>(args)... }
    {
        using Ignore = int[sizeof...(Cpts)];
        (void)Ignore {
            (util::get<Cpts>(m_components) = args, 0)...
        };
    }

    struct VerifyComponent
    {
        Entity<EntityIndex>& self;
        
        template <class Cpt>
        void operator()(Cpt)
        {
            auto cpt_ptr = self.template get_component<Cpt>();
            assert(cpt_ptr != nullptr &&
                   "Component dependency is not initialized!\n");
        }
    };
    
    template <class Cpt>
    void add_component(Ref<Cpt> cpt)
    {
        util::expand_apply<Dependencies<Cpt> >(
            functor::make<VerifyComponent>(*this)
        );
            
        util::get<Ref<Cpt> >(m_components) = cpt;
    }

    
    template <class Cpt>
    Ref<Cpt> get_component()
    { return util::get<Ref<Cpt> >(m_components); }
};

#endif

#include <algorithm>

#include "util.hh"
#include "entity_space.hh"

template <class EntitySpace, class... Routines>
struct System
{
    using EntityType = typename EntitySpace::EntityType;
    using BitMask = std::bitset<EntitySpace::n_components>;
    using BitMasks = std::array<BitMask, sizeof...(Routines)>;

    EntitySpace space;
    BitMasks masks;
    
    template <class Routine>
    constexpr size_t mask_index()
    { return util::index_of<Routine, Routines...>::value; }

    template <class Routine>
    BitMask get_mask()
    { return masks[mask_index<Routine>()]; }
    
    void update()
    {
        typename EntitySpace::Entities subjects;
        BitMask mask;
        
        util::swallow {(
            mask = get_mask<Routines>(),
            std::copy_if(
                space.begin(), space.end(), subjects.begin(),
                [&] (EntityType& ent) -> bool {
                    return ent.supports(mask);
                }),
            Routines::run(subjects.begin(), subjects.end()),
            subjects.clear(),
        0)...};
        
    }
};

template <class... Components>
struct Logic
{
    template <class EntityIt>
    void run(EntityIt begin, EntityIt end)
    {
        for (auto it = begin; it != end; ++it) {
            
            util::swallow {(
                assert(it->template has_component<Components>()),
            0)...};
            
            operate(it->template get_component<Components>()...);
        }
    }

    void operate(Components&&... cs);
};

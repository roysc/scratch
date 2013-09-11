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

    System()
    {
        BitMask mask;
        util::swallow {(
            masks[mask_index<Routines>()] = create_mask<Routines>(),
            0)...};
    }
    
    template <class Routine>
    constexpr size_t get_index()
    { return util::index_of<Routine, Routines...>::value; }

    template <class Routine>
    BitMask get_mask()
    { return masks[get_index<Routine>()]; }

    /** Create a new entity inside the system */
    template <class... InitCpts>
    EntityID create_entity()
    {
        EntityType entity(Ref<InitCpts> {new InitCpts}...);
        return space.insert(std::move(entity));
    }

    // to avoid sys.template ... cruft
    template <class... InitCpts>
    friend EntityID create_entity(System& sys)
    { return sys.template create_entity<InitCpts...>(); }

    
    void update()
    {
        typename EntitySpace::Entities subjects;
        BitMask mask;
        // auto supports = std::mem_fn(&EntityType::supports);
        
        util::swallow {(
            mask = get_mask<Routines>(),
            std::copy_if(
                space.begin(), space.end(), subjects.begin(),
                // std::bind(supports, _2, mask),
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
    using Index = util::TypeVector<Components...>;

    template <class EntitySpace>
    BitMask create_mask()
    {
        BitMask mask;
        size_t ix;
        
        util::swallow {(
            mask.set(util::index_within<
                     Components, typename EntitySpace::Index>::value),
        0)...};
            
        return mask;
    }

    
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
    
    void operate(Components&... cs);
};

#include <algorithm>
#include <functional>

#include "util.hh"
#include "entity_space.hh"
#include "range.hh"

template <class Space, class... Laws>
struct System
{
    using EntityType = typename Space::EntityType;
    using BitMask = std::bitset<Space::max_components>;
    using BitMasks = std::array<BitMask, sizeof...(Laws)>;
    using Routines = util::TypeVector<Laws...>;

    Routines routines;
    Space space;
    BitMasks masks;

    System()
    {
        BitMask mask;
        util::swallow {(
            masks[util::index_of<Laws, Laws...>::value] =
                Laws::template create_mask<Space>(),
            
        0)...};
        
        
    }

    template <class Law>
    BitMask get_mask()
    { return masks[util::index_of<Law, Laws...>::value]; }

    /** Create a new entity with default components */
    template <class... InitCpts>
    EntityID create_entity()
    {
        EntityType entity(InitCpts {}...);
        return space.insert(std::move(entity));
    }

    /** Create a new entity with specified components */
    template <class... InitCpts>
    EntityID create_entity(InitCpts&&... cpts)
    {
        EntityType entity(std::forward<InitCpts>(cpts)...);
        return space.insert(std::move(entity));
    }

    // // to avoid sys.template ... cruft
    // template <class... InitCpts>
    // friend EntityID create_entity(System& sys)
    // { return sys.template create_entity<InitCpts...>(); }

    template <class L>
    L& get_law()
    { return util::get<L>(routines); }

    
    void update()
    {
        BitMask mask;
        auto supports =
            [&] (const EntityType& ent) { return ent.supports(mask); };
        
        util::swallow {(
            mask = get_mask<Laws>(),
            get_law<Laws>().run(
                range::filter(supports, space.begin(), space.end())),
            
        0)...};
        
    }
};

template <class... Components>
struct Logic
{
    using Index = util::TypeVector<Components...>;

    template <class Space>
    static typename Space::BitMask create_mask()
    {
        typename Space::BitMask mask;
        
        util::swallow {(
            mask.set(util::index_within<
                     Components, typename Space::Index>::value),
        0)...};
            
        return mask;
    }

    template <class Entities>
    void run(Entities ents)
    {
        println("Running Logic");
        
        for (auto& ent : ents) {
            println(ent);
            
            util::swallow {(
                assert(ent.template has_component<Components>()),
            0)...};
            
            operate(ent.template get_component<Components>()...);
        }
    }

    
    virtual void operate(Components&... cs)
    {
        println("Empty Logic operation");
    }

    std::string to_string() const
    {
        using util::io::print_to;
        
        std::stringstream out;
        out << "Logic<";
        
        bool at_0 = true;
        util::swallow {(
                print_to(out, at_0 ? "" : ", ",
                         name<Components>()),
                at_0 = false,
                0)...};
        
        out << ">";
        return out.str();
    }
};

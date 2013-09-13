#include <algorithm>
#include <functional>

#include "util.hh"
#include "entity_space.hh"
#include "range.hh"

template <class Space, class... Laws>
struct System
{
    using EntityType = typename Space::EntityType;
    using BitMask = std::bitset<Space::n_components>;
    using BitMasks = std::array<BitMask, sizeof...(Laws)>;
    using Routines = util::TypeVector<Laws...>;

    Routines routines;
    Space space;
    BitMasks masks;

    System()
        // : routines(Routines(space)...)
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
        // std::vector<EntityType> subjects;
        // std::vector<std::reference_wrapper<EntityType> > subjects;
        BitMask mask;
        auto supports =
            [&] (const EntityType& ent) { return ent.supports(mask); };
            // std::bind(std::mem_fn(&EntityType::supports));
        // using View = typename
        //     range::FilterRange<decltype(supports),
        //                        typename Space::iterator>;
        // auto subjects =
        //     range::filter(supports, space.begin(), space.begin());
        
        util::swallow {(
            mask = get_mask<Laws>(),
            // std::copy_if(
            //     space.begin(), space.end(), std::back_inserter(subjects),
            //     supports),
            // get_law<Laws>().run(subjects.begin(), subjects.end()),
            get_law<Laws>().run(
                range::filter(space.begin(), space.end(), supports)),
            // subjects.clear(),
            
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
        size_t ix;
        
        util::swallow {(
            mask.set(util::index_within<
                     Components, typename Space::Index>::value),
        0)...};
            
        return mask;
    }

    template <class EntityIt>
    void run(EntityIt it, EntityIt end)
    {
        println("Running Logic");
        
        for (; it != end; ++it) {
            // typename EntityIt::value_type::type& ent = *it;
            auto& ent = *it;
            
            util::swallow {(
                assert(ent.template has_component<Components>()),
            0)...};
            
            operate(ent.template get_component<Components>()...);
        }
    }

    template <class Range>
    void run(Range r)
    { run(r.begin(), r.end()); }

    
    virtual void operate(Components&... cs)
    {
        println("Base Logic::operate");
    }

    std::string to_string() const
    {
        std::stringstream out;
        bool at_0 = true;
        util::swallow {(
                print_to(out, at_0 ? "" : ", ",
                         name<Components>()),
                at_0 = false,
                0)...};
        return out.str();
    }
};

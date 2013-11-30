#include <utility>
#include <type_traits>
#include <memory>
#include <bitset>
#include <cassert>

#include "util/util.hh"
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

using EntityID = uint;

/**** Entity ****
 *  Anything that "exists" within the system 
 *  Contains a simple aggregation of components
 */
template <class... Components>
struct Entity
{
  // using Contents = util::TypeVector<Ref<Components>...>;
  using Contents = util::TypeVector<Components...>;
  Contents m_components;

  static const size_t max_components = sizeof...(Components);
  using BitMask = std::bitset<max_components>;
  BitMask m_description;

  template <class... Cpts>
  Entity(Cpts&&... args)
  // : m_components { std::forward<Cpts>(args)... }
  {
    util::swallow {(
        util::get<Cpts >(m_components) =
        std::forward<Cpts >(args),
        // println("setting Component ", *args),
            
        0)...};

    util::swallow {(
        m_description.set(util::index_of<Cpts, Components...>::value),
        0)...};

    // println(m_description);
  }

  bool is_empty() const { return m_description.none(); }

  template <class Cpt>
  void add_component(Cpt&& cpt)
  {
    util::get<Cpt >(m_components) = std::move(cpt);
    m_description.set(util::index_of<Cpt, Components...>::value);
  }

  template <class Cpt>
  bool has_component() const
  {
    auto ix = util::index_of<Cpt, Components...>::value;
    return m_description.test(ix);
  }
    
  template <class Cpt>
  Cpt& get_component()
  { return util::get<Cpt>(m_components); }

  template <class Cpt>
  const Cpt& get_component() const
  { return util::get<Cpt>(m_components); }
    
    
  bool supports(BitMask mask) const
  {
    return (mask & m_description) == mask;
  }
    
  std::string to_string() const
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

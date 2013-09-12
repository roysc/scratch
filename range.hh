#include <iterator>
#include <type_traits>

#ifndef _SCRATCH_RANGE
#define _SCRATCH_RANGE

namespace range
{
template <class Predicate, class IterBase>
struct FilterRange
{
    struct iterator
        : std::iterator_traits<IterBase>
        // : IterBase
    {
        using value_type      =
            typename std::iterator_traits<IterBase>::value_type;
        using reference       =
            typename std::iterator_traits<IterBase>::reference;
        using pointer         =
            typename std::iterator_traits<IterBase>::pointer;
        using difference_type =
            typename std::iterator_traits<IterBase>::difference_type;

        Predicate m_pred;
        IterBase m_base, m_end;
        
        iterator(Predicate pred, IterBase it, IterBase end)
            : m_pred(pred), m_base(it), m_end(end)
        { satisfy_predicate(); }
        
        reference operator*() const { return *m_base; }

        FilterRange& operator++()
        {
            ++m_base;
            satisfy_predicate();
        }

        bool operator==(iterator& that)
        {
            return m_base == that.m_base;
        }

        bool operator!=(iterator& that)
        { return !(*this == that); }
    
        void satisfy_predicate()
        {
            while (m_base != m_end && !m_pred(*m_base))
                ++m_base;
        }
    };
    
    FilterRange(Predicate f, IterBase it, IterBase end)
        : m_pred(f), m_iter(it), m_end(end)
    { }

    iterator begin() const { return iterator(m_pred, m_iter, m_end); }
    iterator end() const { return iterator(m_pred, m_end, m_end); }

    Predicate m_pred;
    IterBase m_iter, m_end;
};

template <class Predicate, class Iterator>
FilterRange<Predicate, Iterator>&&
filter(Iterator it, Iterator end, Predicate&& f)
{
    return FilterRange<Predicate, Iterator>(
        std::forward<Predicate>(f), it, end);
}
}

#endif

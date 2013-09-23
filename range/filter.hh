#ifndef _SCRATCH_RANGE_FILTER
#define _SCRATCH_RANGE_FILTER

namespace range
{
    /** Lazily yields elements of an underlying range which satisfy a 
        predicate. */
    template <class Predicate, class R>
    struct FilterRange
    {
        using BaseTraits = typename R::BaseTraits;
        using ElementType = typename BaseTraits::value_type;
        // using Reference = ElementType&;
        using Reference = typename BaseTraits::reference;

        template <class = std::enable_if_t<is_range<R>::value> >
        FilterRange(const Predicate& f, const R& r)
            : m_pred(f), m_input(r)
        {
            satisfy_predicate();
        }
        
        Reference front() const { return m_input.front(); }

        void pop_front()
        {
            m_input.pop_front();
            satisfy_predicate();
        }

        bool has_next() const { return m_input.has_next(); }

      private:
        void satisfy_predicate()
        {
            while (m_input.has_next() && !m_pred(m_input.front()))
                m_input.pop_front();
        }
        
      private:
        Predicate m_pred;
        R m_input;
    };

    template <class Predicate, class R,
              class = std::enable_if_t<is_range<R>::value> >
    FilterRange<Predicate, R>
    filter(Predicate&& f, const R& r)
    {
        return FilterRange<Predicate, R>(std::forward<Predicate>(f), r);
    }
    
    template <class Predicate, class It>
    FilterRange<Predicate, AsRange<It> >
    filter(Predicate&& f, It it, It end)
    {
        return FilterRange<Predicate, AsRange<It> >(
            std::forward<Predicate>(f), as_range(it, end));
    }

    
}

#endif

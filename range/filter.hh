#ifndef _SCRATCH_RANGE_FILTER
#define _SCRATCH_RANGE_FILTER

namespace range
{
    // template <class Predicate, class It>
    template <class Predicate, class R>
    struct FilterRange
    {
        // using BaseTraits = std::iterator_traits<It>;
        using BaseTraits = typename R::BaseTraits;
        using ElementType = typename BaseTraits::value_type;
        using Reference = ElementType&;

        template <class = std::enable_if_t<is_range<R>::value> >
        FilterRange(const Predicate& f, const R& r)
            : m_pred(f), m_input(r)
        {
            satisfy_predicate();
        }
        
        // Reference front() const { return *m_cursor; }
        Reference front() const { return m_input.front(); }

        void pop_front()
        {
            // ++m_cursor;
            m_input.pop_front();
            satisfy_predicate();
        }

        // bool has_next() const { return m_cursor != m_end; }
        bool has_next() const { return m_input.has_next(); }

      private:
        void satisfy_predicate()
        {
            // while (m_cursor != m_end && !m_pred(*m_cursor))
            //     ++m_cursor;

            while (m_input.has_next() && !m_pred(m_input.front()))
                m_input.pop_front();
        }
        
      private:
        Predicate m_pred;
        // It m_cursor, m_end;
        R m_input;
    };

    template <class Predicate, class R>
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
            // std::forward<Predicate>(f), it, end);
            std::forward<Predicate>(f), as_range(it, end));
    }

    
}

#endif

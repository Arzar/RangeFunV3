#pragma once

#include <range/v3/range_facade.hpp>
#include "day.hpp"


template <class ForwardIterator, class BinaryPredicate>
ForwardIterator adjacent_equal(ForwardIterator first, ForwardIterator last, BinaryPredicate pred)
{
    if (first != last)
    {
        ForwardIterator i = first;
        while (++i != last)
        {
            if (pred(*first, *i) == false)
                return i;
            first = i;
        }
    }
    return last;
}

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename InputIterator>
            struct chunkby_sentinel;

            template<typename InputRange, typename BinaryPred>
            struct chunkby_cursor
            {
                using InputIterator = ranges::range_iterator_t<InputRange>;

                InputRange subrng_;
                InputIterator end_;
                BinaryPred pred_;

                chunkby_cursor(InputRange r, BinaryPred pred)
                  : subrng_(r.begin(), r.begin()), end_(r.end()), pred_(pred)
                {
                    move_to_next_subrange();
                }

                const InputRange& current() const
                {
                    return subrng_;
                }

                void next()
                {
                    move_to_next_subrange();
                }

            private:
                void move_to_next_subrange()
                {
                   auto begin_new_subrange = subrng_.end();
                   auto end_new_subrange = adjacent_equal(begin_new_subrange, end_, pred_);
                   subrng_ = InputRange(begin_new_subrange, end_new_subrange);
                }
            };

            template<typename InputRange>
            struct chunkby_sentinel
            {
                using InputIterator = ranges::range_iterator_t<InputRange>;
                InputIterator end_;

                chunkby_sentinel(InputRange r)
                  : end_(r.end())
                {}

                template<typename BinaryPredicate>
                bool equal(chunkby_cursor<InputRange, BinaryPredicate> const &that) const
                {
                    return end_ == that.subrng_.begin();
                }
            };

        }

        template<typename InputRange, typename BinaryPred>
        struct chunkBy_view
          : range_facade<chunkBy_view<InputRange, BinaryPred>>
        {
        private:
            friend range_core_access;
            using InputIterator = ranges::range_iterator_t<InputRange>;

            InputRange r_;
            BinaryPred pred_;

            detail::chunkby_cursor<InputRange, BinaryPred> begin_cursor() const
            {
                return {r_, pred_};
            }
            detail::chunkby_sentinel<InputRange> end_cursor() const
            {
                return {r_};
            }

        public:

            chunkBy_view(InputRange r, BinaryPred pred)
              : r_(r), pred_(pred)
            {}
        };

        namespace view
        {
            struct chunkByer : ranges::bindable<chunkByer>
            {
            private:
                template<typename BinaryPred>
                struct chunkByer1 : ranges::pipeable<chunkByer1<BinaryPred>>
                {
                private:
                    BinaryPred pred_;
                public:
                    chunkByer1(BinaryPred pred)
                      : pred_(std::move(pred))
                    {}
                    template<typename InputRange, typename This>
                    static chunkBy_view<InputRange, BinaryPred>
                    pipe(InputRange rng, This && this_)
                    {
                        return {rng, std::forward<This>(this_).pred_};
                    }
                };
            public:
                template<typename InputRange1, typename BinaryPred>
                static chunkBy_view<InputRange1, BinaryPred>
                invoke(chunkByer, InputRange1 rng, BinaryPred pred)
                {
                    CONCEPT_ASSERT(ranges::Range<InputRange1>());
                    return {rng, std::move(pred)};
                }

                /// \overload
                template<typename BinaryPred>
                static chunkByer1<BinaryPred> invoke(chunkByer, BinaryPred pred)
                {
                    return {std::move(pred)};
                }
            };

            RANGES_CONSTEXPR chunkByer chunkBy {};
        }
    } // end v3

} // end ranges


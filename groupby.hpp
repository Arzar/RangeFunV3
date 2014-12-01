#pragma once

#include <range/v3/range_facade.hpp>
#include "day.hpp"
#include "type_name.h"


template <class ForwardIterator, class BinaryPredicate>
ForwardIterator adjacent_equal(ForwardIterator first, ForwardIterator last, BinaryPredicate pred)
{
    if (first != last)
    {
        ForwardIterator i = first;
        while (++i != last)
        {
            if (!pred(*first, *i))
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
        template<typename InputRange, typename BinaryPred>
        class groupBy_view
          : public range_facade<groupBy_view<InputRange, BinaryPred>>
        {

          using BaseInputRange = std::decay_t<InputRange>;
          using InputIterator = decltype(std::declval<BaseInputRange>().begin());
          using InputSentinel = decltype(std::declval<BaseInputRange>().end());
          using SubRange      = ranges::v3::range<InputIterator, InputSentinel>;

          friend range_access;


          SubRange subrng_;
          InputSentinel end_;
          BinaryPred pred_;

          const SubRange & current() const { return subrng_; }
          SubRange & current() { return subrng_; }
          bool done() const { return subrng_.begin() == end_; }
          void next() { move_to_next_subrange(); }
          void move_to_next_subrange()
          {
             auto begin_new_subrange = subrng_.end();
             auto end_new_subrange = adjacent_equal(begin_new_subrange, end_, pred_);
             subrng_ = SubRange(begin_new_subrange, end_new_subrange);
          }
        public:
          groupBy_view() = default;
          groupBy_view(InputRange&& r, BinaryPred pred)
              : subrng_(std::forward<InputRange>(r).begin(), std::forward<InputRange>(r).begin()),
                end_(std::forward<InputRange>(r).end()),
                pred_(pred)
            {
            /*
               std::cout << "BaseInputRange  " << type_name<BaseInputRange>() << '\n';
               std::cout << "InputIterator   " << type_name<InputIterator>() << '\n';
               std::cout << "InputSentinel   " << type_name<InputSentinel>() << '\n';
               std::cout << "SubRange        " << type_name<SubRange>() << '\n';

               std::cout << "BinaryPred      " << type_name<BinaryPred>() << '\n';
            */

               move_to_next_subrange();
            }
        };

        namespace view
        {
            struct groupBy_fn
            {
                template<typename Rng, typename Fun>
                groupBy_view<Rng, Fun> operator()(Rng && rng, Fun fun) const
                {
                    CONCEPT_ASSERT(InputIterable<Rng>());
                    //CONCEPT_ASSERT(Invokable<Fun, range_value_t<Rng>>());
                    return {std::forward<Rng>(rng), std::move(fun)};
                }

                template<typename Fun>
                auto operator()(Fun fun) const ->
                    decltype(make_pipeable(std::bind(*this, std::placeholders::_1, protect(std::move(fun)))))
                {
                    return make_pipeable(std::bind(*this, std::placeholders::_1, protect(std::move(fun))));
                }
            };

            constexpr groupBy_fn groupBy {};
        }
    }
}



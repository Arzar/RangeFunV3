#pragma once

#include <range/v3/range_facade.hpp>
#include <range/v3/view/adjacent_filter.hpp>
#include <range/v3/algorithm/adjacent_find.hpp>

#include "day.hpp"

template <typename InputRange, typename BinaryPred>
struct chunkBy_view
  : ranges::range_facade<chunkBy_view<InputRange, BinaryPred>>
{

private:

    friend ranges::range_facade<chunkBy_view>;
    InputRange rng_;
    BinaryPred pred_;

    decltype(auto) dereference() const
    {
       return rng_ | ranges::view::adjacent_filter(pred_);
    }
        decltype(auto) dereference()
    {
       return rng_ | ranges::view::adjacent_filter(pred_);
    }
    void increment()
    {
       auto it = rng_.begin();
       auto e = rng_.end();
       it = std::adjacent_find(it, e, std::not2(pred_));
       if(it != e)
         ++it;
       rng_ = InputRange(it, e);
    }
    bool done() const
    {
      return rng_.begin() == rng_.end();
    }
    bool equal(chunkBy_view const &that) const
    {
       return rng_.begin() == that.rng_.begin();
    }
public:
    chunkBy_view(InputRange rng, BinaryPred pred)
        : rng_(rng), pred_(pred){}
};

template <typename InputRange, typename BinaryPred>
chunkBy_view<InputRange, BinaryPred> ChunkBy(InputRange ir, BinaryPred pred)
{
   return chunkBy_view<InputRange, BinaryPred>(ir, pred);
}


/*
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
            pipe(InputRange && rng, This && this_)
            {
                return {std::forward<InputRange>(rng), std::forward<This>(this_).pred_};
            }
        };
    public:
        ///
        template<typename InputRange1, typename BinaryPred>
        static chunkBy_view<InputRange1, BinaryPred>
        invoke(chunkByer, InputRange1 && rng, BinaryPred pred)
        {
            //CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
            //CONCEPT_ASSERT(ranges::Callable<invokable_t<UnaryFunction>,
            //                                range_reference_t<InputRange1>>());
            return {std::forward<InputRange1>(rng), std::move(pred)};
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
*/


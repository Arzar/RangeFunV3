#pragma once

#include <range/v3/range_facade.hpp>
#include <range/v3/algorithm/adjacent_find.hpp>

#include "day.hpp"



template <typename InputRange, typename BinaryPred>
struct chunkBy_view
  : ranges::range_facade<chunkBy_view<InputRange, BinaryPred>>
{

private:

    friend ranges::range_core_access;
    InputRange rng_;
    InputRange subrng_;
    BinaryPred pred_;

   	void move_to_next_subrange()
    {
       auto begin_new_subrange = subrng_.end();
       rng_ = InputRange(begin_new_subrange, rng_.end());

       if (begin_new_subrange == rng_.end())
         return;

       auto end_new_subrange = ranges::adjacent_find(rng_, pred_);
       if(end_new_subrange != rng_.end())
         ++end_new_subrange;

       subrng_ = InputRange(begin_new_subrange, end_new_subrange);
    }

    const InputRange& current() const
    {
       return subrng_;
    }

    InputRange& current()
    {
       return subrng_;
    }

    void next()
    {
       	move_to_next_subrange();
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
        : rng_(rng), subrng_(rng.begin(), rng.begin()), pred_(pred)
    {
       move_to_next_subrange();
    }
};

template <typename InputRange, typename BinaryPred>
chunkBy_view<InputRange, BinaryPred> ChunkByFunc(InputRange ir, BinaryPred pred)
{
   return chunkBy_view<InputRange, BinaryPred>(ir, pred);
}



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
        ///
        template<typename InputRange1, typename BinaryPred>
        static chunkBy_view<InputRange1, BinaryPred>
        invoke(chunkByer, InputRange1 rng, BinaryPred pred)
        {
            //CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
            //CONCEPT_ASSERT(ranges::Callable<invokable_t<UnaryFunction>,
            //                                range_reference_t<InputRange1>>());
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


#pragma once

#include <range/v3/range_facade.hpp>
#include <range/v3/view/adjacent_filter.hpp>
#include <range/v3/algorithm/adjacent_find.hpp>

#include "day.hpp"

template <typename BinaryPred>
struct chunkBy
  : ranges::range_facade<chunkBy<BinaryPred>>
{

rivate:

    friend ranges::range_facade<chunkBy>;
    day_iterator c_;
    day_iterator e_;
    decltype(auto) dereference() const
    {
       return ranges::range(c_, e_) | ranges::view::adjacent_filter(BinaryPred());
    }
    void increment()
    {
        c_ = std::adjacent_find(c_, e_, std::not2(BinaryPred()));
        if(c_ != e_)
          ++c_;
    }
    bool done() const
    {
      return *c_ == *e_;
    }
    bool equal(chunkBy const &that) const
    {
       return *c_ == *that.c_;
    }
public:
    chunkBy(day_iterator b, day_iterator e)
        : c_(b), e_(e) {}
};


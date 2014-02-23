#pragma once

#include "chunkby.hpp"
#include "day.hpp"


 struct PredByMonth
 {
    using first_argument_type = greg::date;
    using second_argument_type = greg::date;
    bool operator()(const greg::date& d1, const greg::date& d2) const
    {
      return d1.month() == d2.month();
    }
 };

 struct PredByWeek
 {
    using first_argument_type = greg::date;
    using second_argument_type = greg::date;
    bool operator()(const greg::date& d1, const greg::date& d2) const
    {
      return d1.week_number() == d2.week_number();
    }
 };

template <typename InputRange>
chunkBy_view<InputRange, PredByMonth> ChunkByMonth(InputRange ir)
{
   return ChunkBy(ir, PredByMonth());
}

template <typename InputRange>
chunkBy_view<InputRange, PredByWeek> ChunkByWeek(InputRange ir)
{
   return ChunkBy(ir, PredByWeek());
}







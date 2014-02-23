#pragma once

#include "chunkby.hpp"
#include "day.hpp"


 struct PredMonthEq
 {
    using first_argument_type = greg::date;
    using second_argument_type = greg::date;
    bool operator()(const greg::date& d1, const greg::date& d2) const
    {
      return d1.month() == d2.month();
    }
 };

 using ChunkByMonth = chunkBy<PredMonthEq>;






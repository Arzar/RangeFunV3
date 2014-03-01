#pragma once

#include "chunkby.hpp"
#include "day.hpp"


 struct PredByMonth
 {
    using first_argument_type = greg::date;
    using second_argument_type = greg::date;
    bool operator()(const greg::date& d1, const greg::date& d2) const
    {
      return d1.month() != d2.month();
    }
 };

 struct PredByWeek
 {
    using first_argument_type = greg::date;
    using second_argument_type = greg::date;
    bool operator()(const greg::date& d1, const greg::date& d2) const
    {
      return d1.week_number() != d2.week_number();
    }
 };


template <typename InputRange>
chunkBy_view<InputRange, PredByMonth> ChunkByMonth(InputRange ir)
{
   return ChunkByFunc(ir, PredByMonth());
}

template <typename InputRange>
chunkBy_view<InputRange, PredByWeek> ChunkByWeek(InputRange ir)
{
   return ChunkByFunc((ir), PredByWeek());
}



template <class InputRng, class Value>
Value join(InputRng rng, Value sep)
{
   // Define working types
   using ResultT   = Value;

   // Parse input
   auto itBegin = ranges::begin(rng);
   auto itEnd = ranges::end(rng);

   // Construct container to hold the result
   ResultT result;

// Append first element
   if (itBegin != itEnd)
   {
      result += *itBegin;
      ++itBegin;
   }

   for (; itBegin != itEnd; ++itBegin)
   {
      // Add separator
      result += sep;
      // Add element
      result += *itBegin;
   }

   return result;
}






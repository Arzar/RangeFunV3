#pragma once
#include <range/v3/range_facade.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template <class It, class Sent, class Value>
            typename std::iterator_traits<It>::value_type join(It it, Sent e, Value sep)
            {
               // Define working types
               using ResultT = typename std::iterator_traits<It>::value_type;

               // Construct container to hold the result
               ResultT result;

            // Append first element
               if (it != e)
               {
                  result += *it++;
               }

               for (; it != e; ++it)
               {
                  // Add separator
                  result += sep;
                  // Add element
                  result += *it;
               }

               return result;
            }

        }

       template <class Rng, class Value>
       range_value_t<Rng> join(Rng rng, Value&& sep)
       {
          return detail::join(rng.begin(), rng.end(), std::forward<Value>(sep));
       }
    } // inline namespace v3
}

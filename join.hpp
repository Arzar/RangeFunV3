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

       namespace action
        {
            template <typename Val>
            struct join_fn_impl
            {
               Val sep_;
               join_fn_impl(Val&& s):sep_(std::move(s)){}

               template<typename Rng>
               range_value_t<Rng> operator()(const Rng& rng) const
               {
                  return ranges::join(rng, std::move(sep_));
               }
            };

            struct join_fn
            {
            public:
                template<typename Val>
                join_fn_impl<Val> operator()(Val&& sep) const
                {
                    return join_fn_impl<Val>{std::forward<Val>(sep)};
                }
            };

            constexpr join_fn join{};

            template <typename Range, typename Val>
            auto operator|(const Range& r, join_fn_impl<Val> joiner)
            {
              return joiner(r);
            }
        }

    } // inline namespace v3
}

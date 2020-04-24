#pragma once
#include"range_traits.hpp"
#include"cartezian_product_iterator.hpp"
#include<vector>
#include<algorithm>

//each range must have range adaptor that implements the range and iterator type
namespace ranges {
	namespace view {
		template<typename... Ranges>
		class cartezian_product_range_adapter {
			//generate compile time error if not all the types in
			//the supplied variadic template pack are ranges
			static_assert(RangeTraits::are_all_ranges<Ranges...>::value,
				"Error: all types in the given list of types must be Ranges");
			std::tuple<Ranges...> m_ranges;

		public:
			//typenames that need to exist in each range
			using value_type = std::tuple<typename Ranges::value_type...>;
			using iterator = ranges::internals::iterators::cartezian_product_iterator<typename Ranges::const_iterator...>;

			cartezian_product_range_adapter(const Ranges&... ranges)
				noexcept(noexcept(std::make_tuple(ranges...)))
				:m_ranges(std::make_tuple(ranges...)) {}

			cartezian_product_range_adapter(const cartezian_product_range_adapter<Ranges...>&) = default;
			cartezian_product_range_adapter(cartezian_product_range_adapter<Ranges...>&&) = default;

			auto begin() const {
				//to extract begin iterator the tuple must be accessed and to do that
				//we must use helper function that recieve at compile time all the tuple indicies
				return this->extractBeginEndIteratorsFromRangesTupleToCallBegin(
					std::make_index_sequence<std::tuple_size_v<decltype(this->m_ranges)>>{});
			}

			auto end() const {
				return this->begin().getEndIterator();
			}

			decltype(auto) toVector() const {
				std::vector<value_type> result;
				std::copy(this->begin(), this->end(), std::back_inserter(result));
				return result;
			}

			operator std::vector<value_type>() const {
				return this->toVector();
			}

		private:
			template<std::size_t... indicies>
			auto extractBeginEndIteratorsFromRangesTupleToCallBegin(std::index_sequence<indicies...>) const {
				//using fold expression to generate tuple that contains begin iterator as the begin, begin iterator as the current
				//and end iterator for the end for each range in the tuple range 
				return ranges::internals::iterators::cartezian_product_iterator
				(std::pair(std::get<indicies>(this->m_ranges).cbegin(), std::get<indicies>(this->m_ranges).cend())...);
			}
		};
		//function that accepts variadic amount of ranges and creates cartezian_product_range_adapter from them
		template<typename... Ranges>
		auto cartezian_product(Ranges&&... ranges) {
			static_assert(RangeTraits::are_all_ranges<Ranges...>::value,
				"error: all given types must be range types");
			return ranges::view::cartezian_product_range_adapter{ std::forward<Ranges>(ranges)... };
		}
	}
}
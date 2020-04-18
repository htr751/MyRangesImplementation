#pragma once
#include"range_traits.hpp"
#include"cartezian_product_iterator.hpp"
#include<vector>
#include<algorithm>

namespace ranges {
	namespace view {
		template<typename... Ranges>
		class cartezian_product_range_adapter {
			static_assert(RangeTraits::are_all_ranges<Ranges...>::value,
				"Error: all types in the given list of types must be Ranges");
			std::tuple<Ranges...> m_ranges;

		public:
			using value_type = std::tuple<typename Ranges::value_type...>;
			using iterator = ranges::internals::iterators::cartezian_product_iterator<typename Ranges::iterator...>;

			cartezian_product_range_adapter(const Ranges&... ranges)
				noexcept(noexcept(std::make_tuple(ranges...)))
				:m_ranges(std::make_tuple(ranges...)) {}

			cartezian_product_range_adapter(const cartezian_product_range_adapter<Ranges...>&) = default;
			cartezian_product_range_adapter(cartezian_product_range_adapter<Ranges...>&&) = default;

			auto begin() const {
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
				return ranges::internals::iterators::cartezian_product_iterator
					(std::pair(std::get<indicies>(this->m_ranges).begin(), std::get<indicies>(this->m_ranges).end())...);
			}

			

		};
	}
}
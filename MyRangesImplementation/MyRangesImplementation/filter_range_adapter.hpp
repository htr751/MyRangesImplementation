#pragma once
#include"range_traits.hpp"
#include"filter_iterator.hpp"
#include<type_traits>
#include<vector>

namespace ranges {
	namespace view {
		template<typename Range, typename FilterFunc>
		class filter_range_adapter {
			//create compile time error if the given first type is not a range
			static_assert(RangeTraits::isRange<Range>(), "error: the given first type must be a Range type");
			const Range& m_range;
			FilterFunc m_filter;

		public:
			using value_type = typename Range::value_type;
			using iterator = ranges::internals::iterators::filter_iterator<typename Range::iterator, FilterFunc>;

			filter_range_adapter(const Range& range, FilterFunc filter)
				noexcept(std::is_move_constructible_v<FilterFunc>)
				:m_range(range), m_filter(std::move(filter)) {}

			filter_range_adapter(const filter_range_adapter<Range, FilterFunc>& other)
				noexcept(std::is_copy_constructible_v<FilterFunc>)
				:m_range(other.m_range), m_filter(other.m_filter) {}

			filter_range_adapter(filter_range_adapter<Range, FilterFunc>&& other)
				noexcept(std::is_move_constructible_v<FilterFunc>)
				:m_range(other.m_range), m_filter(std::move(other.m_filter)) {}

			auto begin() const {
				return ranges::internals::iterators::filter_iterator(this->m_range.begin(), this->m_range.end(), this->m_filter);
			}

			auto end() const {
				return ranges::internals::iterators::filter_iterator(this->m_range.end(), this->m_range.end(), this->m_filter);
			}

			decltype(auto) toVector() const {
				std::vector<value_type> result;
				std::copy(this->begin(), this->end(), std::back_inserter(result));
				return result;
			}

			operator std::vector<value_type>() const {
				return this->toVector();
			}
		};
	}
}
//adaptor factory to create filter range factory that accepts in it constructor only a filter function
//to create filter_range_adaptor using the factory operator | must be used and must be supplied with 
// a factory and an underlying range
namespace ranges {
	namespace internals {
		namespace adaptorFactories {
			template<typename FilterFunc>
			class filter_range_adaptor_factory {
				FilterFunc m_filter;
			public:
				filter_range_adaptor_factory(FilterFunc filter)
					noexcept(std::is_nothrow_move_constructible_v<FilterFunc>)
					:m_filter(std::move(filter)) {}

				template<typename Range>
				auto operator()(Range&& range) const {
					static_assert(RangeTraits::isRange<Range>(),
						"error: the given type must be a Range type");
					return ranges::view::filter_range_adapter(std::forward<Range>(range), m_filter);
				}
			};
		}
	}
}

namespace ranges {
	namespace view {
		template<typename FilterFunc>
		auto filter(FilterFunc&& filter) {
			return ranges::internals::adaptorFactories::filter_range_adaptor_factory(
				std::forward<FilterFunc>(filter));
		}
	}
}


template<typename Range, typename FilterFunc>
auto operator|(Range&& range,
	const ranges::internals::adaptorFactories::filter_range_adaptor_factory<FilterFunc>& filterRangeFactory) {
	static_assert(RangeTraits::isRange<Range>(),
		"error: the given first type must be a Range type");
	return filterRangeFactory(std::forward<Range>(range));
}
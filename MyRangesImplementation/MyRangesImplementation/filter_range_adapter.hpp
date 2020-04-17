#pragma once
#include"range_traits.hpp"
#include"filter_iterator.hpp"
#include<type_traits>
#include<vector>

namespace ranges {
	namespace view {

		template<typename Range, typename FilterFunc, typename Stub = std::enable_if_t<RangeTraits::isRange<Range>(), void>>
		class filter_range_adapter {
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

				template<typename Range, typename = std::enable_if_t<RangeTraits::isRange<Range>(), void>>
				auto operator()(Range&& range) const {
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


template<typename Range, typename FilterFunc, typename = std::enable_if_t<RangeTraits::isRange<Range>(), void>>
auto operator|(Range&& range,
	const ranges::internals::adaptorFactories::filter_range_adaptor_factory<FilterFunc>& filterRangeFactory) {
	return filterRangeFactory(std::forward<Range>(range));
}
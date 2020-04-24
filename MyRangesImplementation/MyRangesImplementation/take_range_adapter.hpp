#pragma once
#include"range_traits.hpp"
#include<vector>
#include<algorithm>
//an range adaptor that returns a view of only limit number of elements from the underlying range
namespace ranges {
	namespace view {
		template<typename Range>
		class take_range_adapter {
			static_assert(RangeTraits::isRange<Range>(), "error: the given type must be a range type");
			const Range& m_range;
			int m_limit;
			typename Range::const_iterator endIter;
		public:
			using value_type = typename Range::value_type;
			using iterator = typename Range::const_iterator;

			explicit take_range_adapter(const Range& range, int limit) noexcept : m_range(range), m_limit(limit) {
				auto curIter = this->m_range.begin();
				for (int i = 0; i < m_limit && curIter != this->m_range.end(); i++)
					curIter++;
				this->endIter = curIter;
			}
			take_range_adapter(const take_range_adapter&) = default;
			take_range_adapter(take_range_adapter&&) = default;

			auto begin() const {
				return this->m_range.begin();
			}
			auto end() const {
				return endIter;
			}

			auto toVector() const {
				std::vector<value_type> result;
				std::copy(this->begin(), this->end(), std::back_inserter(result));
				return result;
			}

			operator std::vector<value_type>() const {
				return this->toVector();
			}
		};
	}

	namespace internals {
		namespace adapterFactories {
			class take_range_adapter_factory {
				int m_limit;
			public:
				explicit take_range_adapter_factory(int limit)noexcept :m_limit(limit) {}
				template<typename Range>
				auto operator()(Range&& range) const {
					static_assert(RangeTraits::isRange<Range>(), "error: the given type must be a range type");
					return ranges::view::take_range_adapter(std::forward<Range>(range), m_limit);
				}
			};
		}
	}

	namespace view {
		inline auto take(int limit) noexcept {
			return ranges::internals::adapterFactories::take_range_adapter_factory(limit);
		}
	}
}

template<typename Range>
auto operator|(Range&& range, const ranges::internals::adapterFactories::take_range_adapter_factory& factory) {
	static_assert(RangeTraits::isRange<Range>(), "error: the given type must be a range type");
	return factory(std::forward<Range>(range));
}
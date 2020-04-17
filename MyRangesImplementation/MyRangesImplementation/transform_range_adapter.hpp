#pragma once
#include"range_traits.hpp"
#include"transform_iterator.hpp"
#include<type_traits>
#include<vector>

namespace ranges {
	namespace view {
		template<typename Range, typename TransformFunc, typename Stub = std::enable_if_t<RangeTraits::isRange<Range>(), void>>
		class transform_range_adaptor {
			const Range& m_range;
			TransformFunc m_transform;

			//this static variables remove code duplication in the noexcept clauses
			static constexpr bool is_transformFunc_has_nothrow_move_ctor = std::is_nothrow_move_constructible_v<TransformFunc>;
			static constexpr bool is_transformFunc_has_nothrow_copy_ctor = std::is_nothrow_copy_constructible_v<TransformFunc>;
		public:
			using value_type = std::invoke_result_t<TransformFunc, typename Range::value_type>;
			using iterator = ranges::internals::iterators::transform_iterator<typename Range::iterator, TransformFunc>;

			transform_range_adaptor(const Range& range, TransformFunc func)
				noexcept(is_transformFunc_has_nothrow_move_ctor)
				: m_transform(std::move(func)), m_range(range) {}

			transform_range_adaptor(const transform_range_adaptor<Range, TransformFunc, Stub>& other)
				noexcept(is_transformFunc_has_nothrow_copy_ctor)
				:m_transform(other.m_transform), m_range(other.m_range) {}

			transform_range_adaptor(transform_range_adaptor<Range, TransformFunc, Stub>&& other)
				noexcept(is_transformFunc_has_nothrow_move_ctor)
				:m_transform(std::move(other.m_transform)), m_range(other.m_range) {}

			auto begin() const {
				return ranges::internals::iterators::transform_iterator(this->m_range.begin(), this->m_transform);
			}
			auto end() const {
				return ranges::internals::iterators::transform_iterator(this->m_range.end(), this->m_transform);
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
			template<typename TransformFunc>
			class transform_range_adaptor_factory {
				TransformFunc m_transform;
			public:
				transform_range_adaptor_factory(TransformFunc transform)
					noexcept(noexcept(std::is_nothrow_move_constructible_v<TransformFunc>))
					:m_transform(std::move(transform)) {}

				template<typename Range, typename = std::enable_if_t<RangeTraits::isRange<Range>(), void>>
				auto operator()(Range&& range) const {
					return ranges::view::transform_range_adaptor(std::forward<Range>(range), m_transform);
				}
			};
		}
	}
}

namespace ranges {
	namespace view {
		template<typename TransformFunc>
		auto transform(TransformFunc&& transform) {
			return ranges::internals::adaptorFactories::transform_range_adaptor_factory(
				std::forward<TransformFunc>(transform));
		}
	}
}

template<typename Range, typename TransformFunc, typename = std::enable_if_t<RangeTraits::isRange<Range>(), void>>
auto operator|(Range&& range,
	const ranges::internals::adaptorFactories::transform_range_adaptor_factory<TransformFunc>& transformRangeFactory) {
	return transformRangeFactory(std::forward<Range>(range));
}
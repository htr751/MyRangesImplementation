#pragma once
#include"range_traits.hpp"
#include"zip_iterator.hpp"
#include<type_traits>
#include<vector>

namespace ranges {
	namespace view {

		template<typename FirstRange, typename SecondRange>
		class zip_range_adapter {
			static_assert(RangeTraits::isRange<FirstRange>() && RangeTraits::isRange<SecondRange>(),
				"error: the given two types must be Range types");

			const FirstRange& m_firstRange;
			const SecondRange& m_secondRange;

		public:
			using value_type = std::pair<typename FirstRange::value_type, typename SecondRange::value_type>;
			using iterator = ranges::internals::iterators::zip_iterator<typename FirstRange::iterator, typename SecondRange::iterator>;

			zip_range_adapter(const FirstRange& firstRange, const SecondRange& secondRange) noexcept
				: m_firstRange(firstRange), m_secondRange(secondRange) {}
			zip_range_adapter(const zip_range_adapter<FirstRange, SecondRange>& other) noexcept
				: m_firstRange(other.m_firstRange), m_secondRange(other.m_secondRange) {}

			auto begin() const
				noexcept(std::is_nothrow_constructible_v<ranges::internals::iterators::zip_iterator<
					typename FirstRange::iterator, typename SecondRange::iterator>, typename FirstRange::iterator, typename SecondRange::iterator >)
			{
				return ranges::internals::iterators::zip_iterator(this->m_firstRange.begin(), this->m_secondRange.begin());
			}

			auto end() const
				noexcept(std::is_nothrow_constructible_v<ranges::internals::iterators::zip_iterator<
					typename FirstRange::iterator, typename SecondRange::iterator>, typename FirstRange::iterator, typename SecondRange::iterator >)
			{
				return ranges::internals::iterators::zip_iterator(this->m_firstRange.end(), this->m_secondRange.end());
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
	namespace view {
		template<typename FirstRange, typename SecondRange>
		auto zip(FirstRange&& firstRange, SecondRange&& secondRange) {
			static_assert(RangeTraits::isRange<FirstRange>() && RangeTraits::isRange<SecondRange>(),
				"error: the given two types must be Range types");

			return ranges::view::zip_range_adapter(firstRange, secondRange);
		}
	}
}
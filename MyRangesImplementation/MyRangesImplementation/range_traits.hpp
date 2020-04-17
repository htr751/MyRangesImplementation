#pragma once
#include"typeInformation.hpp"
#include<iterator>

namespace RangeTraits {
	template<typename, typename = void>
	struct is_iterator : std::false_type {};

	template<typename Iterator>
	struct is_iterator<Iterator, std::void_t<std::enable_if_t<
		!std::is_same_v<typename std::iterator_traits<Iterator>::value_type, void>>>> : std::true_type{};

	template<typename... Iterators>
	struct are_all_iterators {
		static constexpr bool value = (... && RangeTraits::is_iterator<Iterators>::value);
	};

	template<typename Range>
	constexpr bool isRange() {
		auto has_begin_detector = [](auto&& range)->decltype(range.begin()) {};
		if constexpr (!typeInformation::template is_valid<Range>(has_begin_detector))
			return false;
		if constexpr (!is_iterator<typename typeInformation::get_expression_type_if_valid<
			decltype(has_begin_detector), Range >::type>::value)
			return false;

		auto has_end_detector = [](auto&& range)->decltype(range.end()) {};
		if constexpr (!typeInformation::template is_valid<Range>(has_end_detector))
			return false;
		if constexpr (!is_iterator <typename typeInformation::get_expression_type_if_valid<
			decltype(has_end_detector), Range >::type >::value)
			return false;

		return true;
	}

	template<typename... Ranges>
	struct are_all_ranges {
		static constexpr bool value = (... && isRange<Ranges>());
	};
}

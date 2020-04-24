#pragma once
#include"typeInformation.hpp"
#include<iterator>

namespace RangeTraits {
	//is_iterator checks if a given type is an iterator at compile time
	//a given type is an iterator if std::iterator_traits can get the type as parameter
	template<typename, typename = void>
	struct is_iterator : std::false_type {};

	template<typename Iterator>
	struct is_iterator<Iterator, std::void_t<std::enable_if_t<
		!std::is_same_v<typename std::iterator_traits<Iterator>::value_type, void>>>> : std::true_type{};

	//check if all the types in a variadic pack of types are iterators
	template<typename... Iterators>
	struct are_all_iterators {
		static constexpr bool value = (... && RangeTraits::is_iterator<Iterators>::value);
	};

	//check if a given type is range. a range is a type with begin and end functions that return iterators
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

	//check if all the types in a variadic pack of types are ranges
	template<typename... Ranges>
	struct are_all_ranges {
		static constexpr bool value = (... && isRange<Ranges>());
	};
}

#pragma once
#include<type_traits>

namespace typeInformation {
	//checks at compile time if a given expression is valid (the detector pattern).
	template<typename Func, typename T>
	struct is_valid_impl {
		template<typename, typename = void>
		struct CheckValidation : std::false_type {};

		template<typename U>
		struct CheckValidation<U, std::void_t<decltype(std::declval<Func>()(std::declval<U>()))>> : std::true_type {};

		static constexpr bool value = CheckValidation<T>::value;
	};

	template<typename T, typename Func>
	constexpr bool is_valid(Func&&) {
		return is_valid_impl<Func, T>::value;
	}

	struct invalid_expression {};

	template<typename, typename, typename = void>
	struct get_expression_type_if_valid {
		using type = invalid_expression;
	};
	//get the type of the result of an expression if it is valid
	template<typename Func, typename T>
	struct get_expression_type_if_valid<Func, T, std::void_t<decltype(std::declval<Func>()(std::declval<T>()))>> {
		using type = typename std::invoke_result_t<Func, T>;
	};
}
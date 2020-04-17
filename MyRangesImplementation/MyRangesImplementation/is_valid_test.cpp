#include"typeInformation.hpp"
#include<catch.hpp>

struct IsValidCheck {
	void func() {}
};

TEST_CASE("check if is_valid returns true for a valid expression", "[is_valid]") {
	constexpr auto result = typeInformation::is_valid<IsValidCheck>([](auto&& val)->decltype(val.func()){});
	REQUIRE(result == true);
}

TEST_CASE("check if is_valid returns false for an invalid expression", "[is_valid]") {
	constexpr auto result = typeInformation::is_valid<IsValidCheck>([](auto&& val)->decltype(val.x){});
	REQUIRE(result == false);
}

TEST_CASE("check if get_expression_type_if_valid returns correct type for valid expression", "[get_expression_type_if_valid]") {
	constexpr auto expressionToCheck = [](auto&& value)->decltype(value.func()) {};
	using result_type = typeInformation::get_expression_type_if_valid<decltype(expressionToCheck), IsValidCheck>::type;
	REQUIRE(std::is_same_v<result_type, void> == true);
}

TEST_CASE("check if get_expression_type_if_valid returns invalid_expression for invalid expression", "[get_expression_type_if_valid]") {
	constexpr auto expressionToCheck = [](auto&& value)->decltype(value.foo()) {};
	using result_type = typeInformation::get_expression_type_if_valid<decltype(expressionToCheck), IsValidCheck>::type;
	REQUIRE(std::is_same_v<result_type, typeInformation::invalid_expression> == true);

}
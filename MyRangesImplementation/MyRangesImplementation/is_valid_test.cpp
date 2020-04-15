#include"MyRanges.hpp"
#include<catch.hpp>

struct IsValidCheck {
	void func() {}
};

TEST_CASE("check if is_valid returns true for a valid expression", "[is_valid]") {
	constexpr auto result = typeValidation::is_valid<IsValidCheck>([](auto&& val)->decltype(val.func()){});
	REQUIRE(result == true);
}

TEST_CASE("check if is_valid returns false for an invalid expression", "[is_valid]") {
	constexpr auto result = typeValidation::is_valid<IsValidCheck>([](auto&& val)->decltype(val.x){});
	REQUIRE(result == false);
}
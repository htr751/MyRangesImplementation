#include"MyRanges.hpp"
#include<catch.hpp>
struct NonRange {
	void begin();
	void end();
};

TEST_CASE("check if isRange function returns true for a vector range class", "[isRange]") {
	constexpr auto result = RangeTraits::isRange<std::vector<int>>();
	REQUIRE(result == true);
}

TEST_CASE("check if isRange function returns false for non range class like NonRange", "[isRange]") {
	constexpr auto result = RangeTraits::isRange<NonRange>();
	REQUIRE(result == false);
}

TEST_CASE("check if is_iterator returns true for an iterator type like transfrom_iterator", "[is_iterator]") {
	auto transformFunction = [](int x) {return 2 * x; };
	constexpr auto result = RangeTraits::is_iterator<ranges::internals::iterators::transform_iterator<std::vector<int>::iterator, decltype(transformFunction)>>::value;
	REQUIRE(result == true);
}

TEST_CASE("check if is_iterator returns false for a type which isn't iterator", "[is_iterator]") {
	constexpr auto result = RangeTraits::is_iterator<NonRange>::value;
	REQUIRE(result == false);
}


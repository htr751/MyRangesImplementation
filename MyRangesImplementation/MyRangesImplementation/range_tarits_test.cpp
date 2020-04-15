#include"MyRanges.hpp"
#include<catch.hpp>
struct NonRange {
	void begin();
};

TEST_CASE("check if isRange function returns true for a vector range class", "[isRange]") {
	auto result = RangeTraits::isRange<std::vector<int>>();
	REQUIRE(result == true);
}

TEST_CASE("check if isRange function returns false for non range class like NonRange", "[isRange]") {
	auto result = RangeTraits::isRange<NonRange>();
	REQUIRE(result == false);
}
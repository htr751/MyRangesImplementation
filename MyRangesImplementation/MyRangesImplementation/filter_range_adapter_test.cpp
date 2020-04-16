#include"MyRanges.hpp"
#include<catch.hpp>

TEST_CASE("filter_range_adapter_test", "[filter_range_adapter]") {
	auto filterFunc = [](int x) {return x % 2 == 0; };

	SECTION("check if begin == end when creating filter_range_adapter from empty vector") {
		std::vector<int> vec;
		auto filterRange = ranges::view::filter_range_adapter(vec, filterFunc);
		REQUIRE(filterRange.begin() == filterRange.end());
	}

	SECTION("given vector {1,2,3,4,5} check if filtering only even numbers return vector {2,4}") {
		auto vec = std::vector{ 1,2,3,4,5 };
		std::vector<int> evenNumbers = ranges::view::filter_range_adapter(vec, filterFunc);
		REQUIRE(evenNumbers == std::vector<int>{2, 4});
	}
}
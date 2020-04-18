#include"take_range_adapter.hpp"

#include<catch.hpp>

TEST_CASE("check if begin == end for take_range_adapter accepting empty vector with limit of 1000", "[take_range_adapter]") {
	std::vector<int> vec;
	auto takeRange = ranges::view::take_range_adapter(vec, 1000);
	REQUIRE(takeRange.begin() == takeRange.end());
}

TEST_CASE("check if take_range_adapter returns 5 elemenets of vector with 6 elements when the limit is 5", "[take_range_adapter]") {
	auto vec = std::vector{ 1,2,3,4,5,6 };
	auto takeRange = ranges::view::take_range_adapter(vec, 5);
	REQUIRE(takeRange.toVector() == std::vector{ 1,2,3,4,5 });
}

TEST_CASE("check that take_range_adapter with initial vector {1,2,3,4,5,6} and limit 4 returns the vector {1,2,3,4} when casting it to vector", "[take_range_adapter]") {
	auto vec = std::vector{ 1,2,3,4,5,6 };
	std::vector<int> smallerVector = ranges::view::take_range_adapter(vec, 4);
	REQUIRE(smallerVector == std::vector{ 1,2,3,4 });
}

TEST_CASE("check that take function works with operator | to create a take range", "[take_range_adapter]") {
	auto vec = std::vector{ 1,2,3,4,5,6 };
	std::vector<int> smallerVector = vec | ranges::view::take(4);
	REQUIRE(smallerVector == std::vector{ 1,2,3,4 });
}
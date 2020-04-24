#include"generator_range_adapter.hpp"
#include"filter_range_adapter.hpp"
#include<vector>
#include<catch.hpp>

TEST_CASE("check if begin of generator_range_adaptor returns 1 for range of ints starting from 1", "[generator_range_adapter]") {
	auto int_values = ranges::view::generator_range_adapter<int>{ 1, -1 };
	CHECK(*int_values.begin() == 1);
	CHECK(*(++int_values.begin()) == 2);
}

TEST_CASE("check if ints function create all the values of int", "[generator_range_adapter]") {
	std::vector<int> smaller_than_5 = ranges::view::ints(1) |
		ranges::view::filter([](int x) {return x < 5; });
	REQUIRE(smaller_than_5 == std::vector{ 1,2,3,4 });

}
#include"filter_range_adapter.hpp"
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

TEST_CASE("given vector {1,2,3,4,5} check if filtering only odd numbers return vector {1,3,5} when creating the the filter range using filter_range_adaptor_factory", "[filter_range_adaptor]") {
	auto vec = std::vector{ 1,2,3,4,5 };
	std::vector<int> evenNumbers = ranges::internals::adaptorFactories::filter_range_adaptor_factory(
		[](int x) {return x % 2 != 0; })(vec);
	REQUIRE(evenNumbers == std::vector{ 1,3,5 });
}

TEST_CASE("given lvalue vector {1,2,3,4,5,6} check if filtering even numbers using operator | return vector {2,4,6}", "[filter_range_adaptor]") {
	auto vec = std::vector{ 1,2,3,4,5,6 };
	std::vector<int> evenNumbers = vec | ranges::view::filter([](int x) {return x % 2 == 0; });
	REQUIRE(evenNumbers == std::vector{ 2,4,6 });
}

TEST_CASE("given rvalue vector {1,2,3,4,5,6} check if filtering even numbers using operator | return vector {2,4,6}", "[filter_range_adaptor]") {
	std::vector<int> evenNumbers = std::vector{ 1,2,3,4,5,6 } | ranges::view::filter([](int x) {return x % 2 == 0; });
	REQUIRE(evenNumbers == std::vector{ 2,4,6 });
}
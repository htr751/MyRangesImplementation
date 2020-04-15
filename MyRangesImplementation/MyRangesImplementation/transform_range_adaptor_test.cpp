#include"MyRanges.hpp"
#include<catch.hpp>

TEST_CASE("transform_range_adaptor test", "[transform_range_adaptor]") {
	auto transformFunc = [](int value) {return 2 * value; };

	SECTION("check if begin equals end when transform_range_adaptor gets empty vector") {
		std::vector<int> vec;
		auto transform_view = ranges::view::transform_range_adaptor(vec, transformFunc);
		REQUIRE(transform_view.begin() == transform_view.end());
	}
	SECTION("check if transform func is applied in transform_range_adaptor") {
		std::vector<int> vec;
		vec.push_back(1);
		auto transform_view = ranges::view::transform_range_adaptor(vec, transformFunc);
		REQUIRE((*transform_view.begin()) == 2);
	}
	SECTION("check if toVector function of transform_range_adaptor returns expected vector") {
		std::vector<int> vec{ 1,2,3,4,5 };
		auto resultVector = ranges::view::transform_range_adaptor(vec, transformFunc).toVector();
		REQUIRE(resultVector == std::vector<int>{2, 4, 6, 8, 10});
	}
	SECTION("check if casting operator to vector of transform_range_adaptor returns expceted vector") {
		std::vector<int> vec{ 1,2,3,4,5 };
		std::vector<int> resultVector = ranges::view::transform_range_adaptor(vec, transformFunc);
		REQUIRE(resultVector == std::vector<int>{2, 4, 6, 8, 10});
	}
}

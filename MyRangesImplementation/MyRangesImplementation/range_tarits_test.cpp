#include"range_traits.hpp"
#include"transform_iterator.hpp"
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

TEST_CASE("check if are_all_iterators return true for an empty list of iterators", "[are_all_iterators]") {
	constexpr auto result = RangeTraits::are_all_iterators<>::value;
	REQUIRE(result == true);
}

TEST_CASE("check if are_all_iterators return true for list of 3 iterators types", "[are_all_iterators]") {
	constexpr auto result = RangeTraits::are_all_iterators<
		std::vector<int>::iterator, std::list<int>::iterator, std::unordered_map<int, int>::iterator>::value;
	REQUIRE(result == true);
}

TEST_CASE("check if are_all_iterators return false for list of 2 iterators and one non iterators", "[are_all_iterators]") {
	constexpr auto result = RangeTraits::are_all_iterators<
		std::vector<int>::iterator, NonRange, std::list<int>::iterator>::value;
	REQUIRE(result == false);
}
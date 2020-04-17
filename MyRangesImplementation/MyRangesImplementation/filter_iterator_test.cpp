#include"filter_iterator.hpp"
#include<catch.hpp>

namespace Catch {
	template<typename Iterator, typename FilterFunc>
	struct StringMaker<ranges::internals::iterators::filter_iterator<Iterator, FilterFunc>> {
		static std::string convert(const ranges::internals::iterators::filter_iterator<Iterator, FilterFunc>& iter) {
			return StringMaker<decltype(*iter)>().convert(*iter);
		}
	};
}

TEST_CASE("checking filter_iterator functionallity", "[filter_iterator]") {
	auto vec = std::vector{ 1,2,3,4,5 };
	auto filterFunc = [](int x) {return (x % 2) == 0; };
	auto filterIterator = ranges::internals::iterators::filter_iterator(vec.begin(), vec.end(), filterFunc);

	SECTION("checing filter_iterator operator == returns true for two begin iterator") {
		auto checkIterator = ranges::internals::iterators::filter_iterator(vec.begin(), vec.end(), filterFunc);
		REQUIRE(filterIterator == checkIterator);
	}

	SECTION("checking filter_iterator operator == returns false for two unequal iterators") {
		auto checkIterator = filterIterator++;
		REQUIRE(!(checkIterator == filterIterator));
	}

	SECTION("checking filter_iterator operator != returns true for two unequal iterators") {
		auto checkIterator = filterIterator++;
		REQUIRE(checkIterator != filterIterator);
	}

	SECTION("checking filter_iterator operator != returns false for two equal iterators") {
		auto checkIterator = ranges::internals::iterators::filter_iterator(vec.begin(), vec.end(), filterFunc);
		REQUIRE(!(filterIterator != checkIterator));
	}

	SECTION("checking operator * returns 2 on begin of {1,2,3,4,5} given filtering only even numbers") {
		REQUIRE(*filterIterator == 2);
	}

	SECTION("checking operator ++ (prefix and postfix versions) forward the begin of {1,2,3,4,5} to point on 4 given filtering only even numbers") {
		auto temp = filterIterator;
		++filterIterator;
		temp++;
		CHECK(*filterIterator == 4);
		CHECK(*temp == 4);
	}

	SECTION("check that operator ++ (postfix version) returns the former iterator") {
		auto temp = filterIterator++;
		REQUIRE(*temp == 2);
	}

	SECTION("check that two ++ on {1,2,3,4,5} when filtering only even numbers make the itertaor be end iterator") {
		auto end = ranges::internals::iterators::filter_iterator(vec.end(), vec.end(), filterFunc);
		++(++filterIterator);
		REQUIRE(end == filterIterator);
	}
}
#include"zip_iterator.hpp"
#include<catch.hpp>

namespace Catch {
	template<typename FirstIterator, typename SecondIterator>
	struct StringMaker<ranges::internals::iterators::zip_iterator<FirstIterator, SecondIterator>> {
		static std::string convert(const ranges::internals::iterators::zip_iterator<FirstIterator, SecondIterator>& iter) {
			return StringMaker<decltype(*iter)>().convert(*iter);
		}
	};
}


TEST_CASE("checking zip_iterator functionallity", "[zip_iterator]") {
	auto numbers = std::vector{ 1,2,3,4,5 };
	auto names = std::vector<std::string>{ "roy", "tomer", "heinan", "oshrit", "ronny" };

	auto namesWithNumbers = ranges::internals::iterators::zip_iterator(numbers.begin(), names.begin());
	SECTION("checking operator == returns true for two equal begin iterators") {
		auto namesWithNumbers2 = ranges::internals::iterators::zip_iterator(numbers.begin(), names.begin());
		REQUIRE(namesWithNumbers == namesWithNumbers2);
	}

	SECTION("check operator == returns false for two unequal zip iterators") {
		auto namesWithNumbers2 = ranges::internals::iterators::zip_iterator(++numbers.begin(), ++names.begin());
		REQUIRE(!(namesWithNumbers == namesWithNumbers2));
	}

	SECTION("check operator != returns false for two equal begin zip iterators") {
		auto namesWithNumbers2 = ranges::internals::iterators::zip_iterator(numbers.begin(), names.begin());
		REQUIRE_FALSE(namesWithNumbers != namesWithNumbers2);
	}

	SECTION("check operator != returns true for two unequal zip iterators") {
		auto namesWithNumbers2 = ranges::internals::iterators::zip_iterator(++numbers.begin(), ++names.begin());
		REQUIRE(namesWithNumbers != namesWithNumbers2);
	}

	SECTION("check operator * on begin returns pair {1, roy}") {
		REQUIRE(*namesWithNumbers == std::pair{ 1, std::string("roy") });
	}

	SECTION("check operator ++(prefix version) forward iterator to point on {2, tomer}") {
		++namesWithNumbers;
		REQUIRE(*namesWithNumbers == std::pair(2, std::string("tomer")));
	}

	SECTION("check operator++ (postfix version) returns the former iterator and forward te iterator to point to {2, tomer}") {
		auto tmp = namesWithNumbers++;
		CHECK(*tmp == std::pair(1, std::string("roy")));
		CHECK(*namesWithNumbers == std::pair(2, std::string("tomer")));
	}
}
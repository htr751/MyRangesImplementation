#define CATCH_CONFIG_MAIN
#include<catch.hpp>
#include<vector>
#include"MyRanges.hpp"

namespace Catch {
	template<typename Iterator, typename TransformFunc>
	struct StringMaker<ranges::transform_iterator<Iterator, TransformFunc>> {
		static std::string convert(const ranges::transform_iterator<Iterator, TransformFunc>& iter) {
			return StringMaker<decltype(*iter)>().convert(*iter);
		}
	};
}

TEST_CASE("checking transform_iterator functionallity", "[transform_iterator]") {
	std::vector<int> v = { 1,2,3,4,5 };
	auto transformFunc = [](int x) {return 2 * x; };
	ranges::transform_iterator it(v.begin(), transformFunc);

	SECTION("check opreator == of transform_iterator") {
		ranges::transform_iterator it2(v.begin(), transformFunc);
		REQUIRE(it2 == it);
	}

	SECTION("check copy constructor") {
		auto it2(it);
		REQUIRE(it2 == it);
	}

	SECTION("check operator = ") {
		ranges::transform_iterator it2(++v.begin(), transformFunc);
		it2 = it;
		REQUIRE(it2 == it);
	}

	SECTION("check operator != of transform_iterator") {
		ranges::transform_iterator it2(++v.begin(), transformFunc);
		REQUIRE(it2 != it);
	}

	SECTION("check operator++ (prefix version)") {
		ranges::transform_iterator it2(++v.begin(), transformFunc);
		++it;
		REQUIRE(it == it2);
	}

	SECTION("check operator++ (postfix version)") {
		auto tempIterator = it;
		auto it2 = it++;
		REQUIRE(tempIterator == it2);
	}

	SECTION("check operator *") {
		auto value = *it;
		//it points to the first element in the vector which is 1 and by using the transform function
		//the operator should return 2*1 which is 2
		REQUIRE(value == 2);
	}
}
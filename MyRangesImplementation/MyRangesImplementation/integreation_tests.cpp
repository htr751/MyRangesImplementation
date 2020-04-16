#include"MyRanges.hpp"
#include<catch.hpp>

TEST_CASE("given vector {1,2,3,4,5} when transforming the vector by multiplying all elements by 3 and filtering even numbers the resulting vector will be {6, 12}", "[integreation_tests]") {
	auto vec = std::vector{ 1,2,3,4,5 };
	std::vector<int> result = vec | ranges::view::transform([](int x) {return 3 * x; })
		| ranges::view::filter([](int x) {return x % 2 == 0; });

	REQUIRE(result == std::vector{ 6, 12 });
}
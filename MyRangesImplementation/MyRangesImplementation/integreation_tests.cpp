#include"transform_range_adapter.hpp"
#include"filter_range_adapter.hpp"
#include"zip_range_adapter.hpp"
#include<catch.hpp>

TEST_CASE("given vector {1,2,3,4,5} when transforming the vector by multiplying all elements by 3 and filtering even numbers the resulting vector will be {6, 12}", "[integreation_tests]") {
	auto vec = std::vector{ 1,2,3,4,5 };
	std::vector<int> result = vec | ranges::view::transform([](int x) {return 3 * x; })
		| ranges::view::filter([](int x) {return x % 2 == 0; });

	REQUIRE(result == std::vector{ 6, 12 });
}

TEST_CASE("given vector {1,2,3} and list {roy, tomer, heinan} when ziping the vector and the list, transforming it by concating name and number and filter for strings longer than 4 chars get vector {tomer2, heinan3}", "[integreation_tests]") {
	auto numbers = std::vector{ 1,2,3 };
	auto names = std::list<std::string>{ "roy", "tomer", "heinan" };

	std::vector<std::string> result = ranges::view::zip(numbers, names) |
		ranges::view::transform([](auto&& numberWithName) {return numberWithName.second + std::to_string(numberWithName.first); })
		| ranges::view::filter([](auto&& name) {return name.length() > 4; });

	REQUIRE(result == std::vector<std::string>{"tomer2", "heinan3"});
}
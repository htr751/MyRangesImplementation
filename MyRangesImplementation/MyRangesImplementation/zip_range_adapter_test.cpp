#include"zip_range_adapter.hpp"
#include<catch.hpp>

TEST_CASE("given two empty vectors, check if zip_range_adaptor begin == end", "[zip_range_adaptor]") {
	std::vector<int> numbers;
	std::vector<std::string> names;
	auto zip_range = ranges::view::zip_range_adapter(numbers, names);
	REQUIRE(zip_range.begin() == zip_range.end());
}

TEST_CASE("given numbers {1,2,3} and names {roy, tomer, heinan} check if zip_range_adaptor toVector returns {{1, roy}, {2, tomer}, {3, heinan}}") {
	auto numbers = std::vector{ 1,2,3 };
	auto names = std::list<std::string>{ "roy", "tomer", "heinan" };

	auto numberAndNames = ranges::view::zip_range_adapter(numbers, names).toVector();
	REQUIRE(numberAndNames == std::vector{ std::pair{1, std::string("roy")}, std::pair{2, std::string("tomer")}, std::pair{3, std::string("heinan")} });
}

TEST_CASE("given numbers {1,2,3} and names {roy, tomer, heinan} check if zip_range_adapter casting to vector returns {{1, roy}, {2, tomer}, {3, heinan}}") {
	auto numbers = std::vector{ 1,2,3 };
	auto names = std::list<std::string>{ "roy", "tomer", "heinan" };

	std::vector<std::pair<int, std::string>> numberAndNames = ranges::view::zip_range_adapter(numbers, names);
	REQUIRE(numberAndNames == std::vector{ std::pair{1, std::string("roy")}, std::pair{2, std::string("tomer")}, std::pair{3, std::string("heinan")} });
}

TEST_CASE("given numbers {1,2,3} and names {roy, tomer, heinan} check if zip function returns zip_range castable to vector {{1, roy}, {2, tomer}, {3, heinan}}") {
	auto numbers = std::vector{ 1,2,3 };
	auto names = std::list<std::string>{ "roy", "tomer", "heinan" };

	std::vector<std::pair<int, std::string>> numberAndNames = ranges::view::zip(numbers, names);
	REQUIRE(numberAndNames == std::vector{ std::pair{1, std::string("roy")}, std::pair{2, std::string("tomer")}, std::pair{3, std::string("heinan")} });
}
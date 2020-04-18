#include"cartezian_product_range_adapter.hpp"
#include<catch.hpp>

TEST_CASE("check that begin == end when creating cartezian_range_adapter from empty vector and list", "[cartezian_product_range_adapter]") {
	auto vec = std::vector<int>{};
	auto lst = std::list<int>{};
	auto cartezian_product = ranges::view::cartezian_product_range_adapter{ vec, lst };
	REQUIRE(cartezian_product.begin() == cartezian_product.end());
}

TEST_CASE("given vector {1,2} and list {roy, tomer} check that toVector method of cartezian_range_adapter returns {{1, roy}, {2, roy}, {1, tomer}, {2, tomer}}", "[cartezian_product_range_adapter]") {
	auto vec = std::vector{ 1,2 };
	auto lst = std::list<std::string>{ "roy", "tomer" };
	auto cartezian_product = ranges::view::cartezian_product_range_adapter{ vec, lst }.toVector();
	REQUIRE(cartezian_product == std::vector{ std::tuple{1, std::string{"roy"}}, 
		std::tuple{2, std::string{"roy"}}, std::tuple{1, std::string{"tomer"}},
		std::tuple{2, std::string{"tomer"}} });
}

TEST_CASE("given vector {1,2} and list {roy, tomer} check that casting to vector a cartezian_range_adapter returns {{1, roy}, {2, roy}, {1, tomer}, {2, tomer}}", "[cartezian_product_range_adapter]") {
	auto vec = std::vector{ 1,2 };
	auto lst = std::list<std::string>{ "roy", "tomer" };
	auto cartezian_product_range = ranges::view::cartezian_product_range_adapter{ vec, lst };
	auto cartezian_product_vector = (std::vector<decltype(cartezian_product_range)::value_type>)cartezian_product_range;
	REQUIRE(cartezian_product_vector == std::vector{ std::tuple{1, std::string{"roy"}},
		std::tuple{2, std::string{"roy"}}, std::tuple{1, std::string{"tomer"}},
		std::tuple{2, std::string{"tomer"}} });
}


#include"generator_iterator.hpp"
#include<catch.hpp>

TEST_CASE("check that generator_iterator operator == returns true for two generators starting with the value 1", "[generator_iterator]") {
	auto firstGen = ranges::internals::iterators::generator_iterator{ 1 };
	auto secondGen = ranges::internals::iterators::generator_iterator{ 1 };
	REQUIRE(firstGen == secondGen);
}

TEST_CASE("check that generator_iterator operator == returns false for two generators starting with the values 1 and 2", "[generator_iterator]") {
	auto firstGen = ranges::internals::iterators::generator_iterator{ 2 };
	auto secondGen = ranges::internals::iterators::generator_iterator{ 1 };
	REQUIRE_FALSE(firstGen == secondGen);
}

TEST_CASE("check that generator_iterator operator != returns true for two generators starting with the values 1 and 2", "[generator_iterator]") {
	auto firstGen = ranges::internals::iterators::generator_iterator{ 2 };
	auto secondGen = ranges::internals::iterators::generator_iterator{ 1 };
	REQUIRE(firstGen != secondGen);
}
TEST_CASE("check that generator_iterator operator != returns false for two generators starting with the value 1", "[generator_iterator]") {
	auto firstGen = ranges::internals::iterators::generator_iterator{ 1 };
	auto secondGen = ranges::internals::iterators::generator_iterator{ 1 };
	REQUIRE_FALSE(firstGen != secondGen);
}

TEST_CASE("given generator_iterator of int start from 1, check that operator * return 1", "[generator_iterator]") {
	auto gen = ranges::internals::iterators::generator_iterator{ 1 };
	REQUIRE(*gen == 1);
}

TEST_CASE("given generator_iterator of int start from 1, check that calling operator ++ twice forward it to 3", "[generator_iterator]") {
	auto gen = ranges::internals::iterators::generator_iterator{ 1 };
	++(++gen);
	REQUIRE(*gen == 3);
}

TEST_CASE("given generator_iterator of int start from 1, check that calling opertaor ++ (postfix version) returns the former value and advance to iterator to 2", "[generator_iterator]") {
	auto gen = ranges::internals::iterators::generator_iterator{ 1 };
	auto gen2 = gen++;
	CHECK(*gen == 2);
	CHECK(*gen2 == 1);
}
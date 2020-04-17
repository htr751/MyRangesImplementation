#include"cartezian_product_iterator.hpp"
#include<catch.hpp>


namespace Catch {
	template<typename... Iterators>
	struct StringMaker<ranges::internals::iterators::cartezian_product_iterator<Iterators...>> {
		static std::string convert(const ranges::internals::iterators::cartezian_product_iterator<Iterators...>& iter) {
			return StringMaker<decltype(*iter)>().convert(*iter);
		}
	};
}

TEST_CASE("checking cartezian_product_iterator functionallity", "[cartezian_product_iterator]") {
	auto vec = std::vector{ 1,2 };
	auto lst = std::list{ 3,4 };

	auto vecProductLstIterator = ranges::internals::iterators::cartezian_product_iterator(
		std::pair{ vec.begin(), vec.end() }, std::pair{ lst.begin(), lst.end() }
	);

	SECTION("check that given two iterators points to the begin of the {vec, lst} pair, operator == returns true", "[cartezian_product_iterator]") {
		auto vecProductLstIterator2 = ranges::internals::iterators::cartezian_product_iterator(
			std::pair{ vec.begin(), vec.end() }, std::pair{ lst.begin(), lst.end() }
		);
		REQUIRE(vecProductLstIterator == vecProductLstIterator2);
	}

	SECTION("check that given two different iterators operator = returns false", "[cartezian_product_iterator]") {
		auto vecProductLstIterator2 = ranges::internals::iterators::cartezian_product_iterator(
			std::pair{ ++vec.begin(), vec.end() }, std::pair{ lst.begin(), lst.end() }
		);
		REQUIRE_FALSE(vecProductLstIterator == vecProductLstIterator2);
	}

	SECTION("check that given two different iterators operator != returns true", "[cartezian_product_iterator]") {
		auto vecProductLstIterator2 = ranges::internals::iterators::cartezian_product_iterator(
			std::pair{ ++vec.begin(), vec.end() }, std::pair{ lst.begin(), lst.end() }
		);
		REQUIRE(vecProductLstIterator != vecProductLstIterator2);
	}

	SECTION("check that given two iterators points to the begin of the {vec, lst} pair, operator != returns false", "[cartezian_product_iterator]") {
		auto vecProductLstIterator2 = ranges::internals::iterators::cartezian_product_iterator(
			std::pair{ vec.begin(), vec.end() }, std::pair{ lst.begin(), lst.end() }
		);
		REQUIRE_FALSE(vecProductLstIterator != vecProductLstIterator2);
	}

	SECTION("check that operator * on vecProductLstIterator returns pair of {1, 3}", "[cartezian_product_iterator]") {
		auto vecLstPair = *vecProductLstIterator;
		REQUIRE(vecLstPair == std::tuple{ 1,3 });
	}

	SECTION("check operator++ (prefix version)", "[cartezian_product_iterator]") {
		++vecProductLstIterator;
		CHECK(*vecProductLstIterator == std::tuple{2, 3});
		++vecProductLstIterator;
		CHECK(*vecProductLstIterator == std::tuple{1, 4});
		++vecProductLstIterator;
		CHECK(*vecProductLstIterator == std::tuple{2, 4});
	}

	SECTION("check operator ++(postfix version)", "[cartezian_product_iterator]") {
		auto tmp = vecProductLstIterator;
		auto formerValue = vecProductLstIterator++;
		REQUIRE(formerValue == tmp);
	}
}
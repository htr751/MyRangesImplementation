#pragma once
#include<type_traits>
#include"range_traits.hpp"

namespace ranges {
	namespace internals {
		namespace iterators {
			template<typename... Iterators>
			class cartezian_product_iterator {
				static_assert(RangeTraits::are_all_iterators<Iterators...>::value,
					"error: all template parameters should be iterators" );

				std::tuple<std::tuple<Iterators, Iterators, Iterators>...> m_beginCurrenEndIterTuples;

			public:
				using iterator_category = std::input_iterator_tag;
				using value_type = std::tuple<typename std::iterator_traits<Iterators>::value_type...>;
				using difference_type = std::ptrdiff_t;
				using pointer = std::add_pointer_t<value_type>;
				using reference = std::add_lvalue_reference_t<value_type>;

				explicit cartezian_product_iterator(const std::pair<Iterators, Iterators>&... beginEndPairs)
					noexcept(std::is_constructible_v<std::tuple<std::tuple<Iterators, Iterators, Iterators>...>, const std::tuple<Iterators, Iterators, Iterators>&...>)
					:m_beginCurrenEndIterTuples(std::make_tuple(std::make_tuple(beginEndPairs.first, beginEndPairs.first, beginEndPairs.second)...)) {}

				cartezian_product_iterator() = delete;
				cartezian_product_iterator(const cartezian_product_iterator<Iterators...>&) = default;
				cartezian_product_iterator(cartezian_product_iterator<Iterators...>&&) = default;

				cartezian_product_iterator<Iterators...>& operator=(const cartezian_product_iterator<Iterators...>&) = default;
				cartezian_product_iterator<Iterators...>& operator=(cartezian_product_iterator<Iterators...>&&) = default;

				cartezian_product_iterator<Iterators...>& operator++() {
					advanceCartezianIterator(std::make_index_sequence<
						std::tuple_size_v<decltype(this->m_beginCurrenEndIterTuples)>
					>{});
					return *this;
				}

				cartezian_product_iterator<Iterators...> operator++(int) {
					auto formerValue = *this;
					++(*this);
					return formerValue;
				}

				bool operator==(const cartezian_product_iterator<Iterators...>& other) const {
					return this->m_beginCurrenEndIterTuples == other.m_beginCurrenEndIterTuples;
				}

				bool operator!=(const cartezian_product_iterator<Iterators...>& other) const {
					return !(*this == other);
				}

				decltype(auto) operator*() const {
					return makeTupleOfIteratorValues(std::make_index_sequence<
						std::tuple_size_v<decltype(this->m_beginCurrenEndIterTuples)>>{});
				}

			private:
				template<std::size_t... indicies>
				decltype(auto) makeTupleOfIteratorValues(std::index_sequence<indicies...>)const {
					return std::make_tuple((*std::get<1>(std::get<indicies>(this->m_beginCurrenEndIterTuples)))...);
				}

				template<std::size_t... indicies>
				void advanceCartezianIterator(std::index_sequence<indicies...>) {
					makeCartezianIteratorPointToNextElement(std::get<indicies>(this->m_beginCurrenEndIterTuples)...);
				}

				static bool makeCartezianIteratorPointToNextElement() { return false; }

				template<typename FirstIterator, typename... OtherIterators>
				static bool makeCartezianIteratorPointToNextElement(
					std::tuple<FirstIterator, FirstIterator, FirstIterator>& firstIterTuple,
					std::tuple<OtherIterators, OtherIterators, OtherIterators>&... otherIterTuples) {

					//if before incrementing the current iterator == end iterator
					//then it means the whole iterator points to the end
					if (std::get<1>(firstIterTuple) == std::get<2>(firstIterTuple))
						return false;
					++std::get<1>(firstIterTuple);
					//if afer incrementig the current iterator == end iterator
					//then we need to increment the first iterator in otherIterators recursively
					if (std::get<1>(firstIterTuple) == std::get<2>(firstIterTuple)) {
						if (makeCartezianIteratorPointToNextElement(otherIterTuples...)) {
							std::get<1>(firstIterTuple) = std::get<0>(firstIterTuple);
							return true;
						}
						else
							return false;
					}
					return true;
				}
			};
		}
	}
}

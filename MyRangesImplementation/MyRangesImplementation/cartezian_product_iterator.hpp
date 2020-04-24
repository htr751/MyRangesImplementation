#pragma once
#include<type_traits>
#include"range_traits.hpp"

//cartezian product iterator implementation. 
//a range is somthing that has begin function and end function that return iterators
//this iterator is an implementation of an iterator that unique to the cartezian_product_range
namespace ranges {
	namespace internals {
		namespace iterators {
			template<typename... Iterators>
			class cartezian_product_iterator {
				//there will be a compile time error if not all the types in the
				//supplied variadic pack are iterators
				static_assert(RangeTraits::are_all_iterators<Iterators...>::value,
					"error: all template parameters should be iterators");
				//this data member contain the begin iterator, current iterator and end iterator for 
				//each range the cartezian_product_iterator is going to work on
				//this data is essential because in cartezian product the begin and end 
				//iterators must be save to be able to return to them
				//also a current iterator is essential so that the implementation will know 
				//what is the current element of each range we are currently at
				std::tuple<std::tuple<Iterators, Iterators, Iterators>...> m_beginCurrenEndIterTuples;

			public:
				//typenames that must be in each itrerator
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
					//to be able to access the data in the tuple data member we must use an helper
					//function that accepts at compile time all the indicies of the of the tuple
					//(kind of for loop at compile time)
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
					//same as operator ++, to be able to access the tuple, we must
					//use an helper function that accepts all the indicies of the tuple
					return makeTupleOfIteratorValues(std::make_index_sequence<
						std::tuple_size_v<decltype(this->m_beginCurrenEndIterTuples)>>{});
				}

				auto getEndIterator() const {
					auto copyOfThis = *this;
					copyOfThis.setAllCurrentIteratorsToTheirEnd(
						std::make_index_sequence<std::tuple_size_v<decltype(copyOfThis.m_beginCurrenEndIterTuples)>>{});
					return copyOfThis;
				}

			private:
				template<std::size_t... indicies>
				void setAllCurrentIteratorsToTheirEnd(std::index_sequence<indicies...>) {
					//lambda that set the current iterator of some iterator tuple to the end iterator
					auto setCurrentIteratorsToEnd = [](auto& iterTuple) {
						std::get<1>(iterTuple) = std::get<2>(iterTuple);
					};
					//apply the lambda on all the itrators in a fold expression
					(setCurrentIteratorsToEnd(std::get<indicies>(this->m_beginCurrenEndIterTuples)), ...);
				}

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

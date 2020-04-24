#pragma once
#include<type_traits>

namespace typeInformation {
	template<typename Func, typename T>
	struct is_valid_impl {
		template<typename, typename = void>
		struct CheckValidation : std::false_type {};

		template<typename U>
		struct CheckValidation<U, std::void_t<decltype(std::declval<Func>()(std::declval<U>()))>> : std::true_type {};

		static constexpr bool value = CheckValidation<T>::value;
	};

	template<typename T, typename Func>
	constexpr bool is_valid(Func&&) {
		return is_valid_impl<Func, T>::value;
	}

	struct invalid_expression {};

	template<typename, typename, typename = void>
	struct get_expression_type_if_valid {
		using type = invalid_expression;
	};

	template<typename Func, typename T>
	struct get_expression_type_if_valid<Func, T, std::void_t<decltype(std::declval<Func>()(std::declval<T>()))>> {
		using type = typename std::invoke_result_t<Func, T>;
	};
}

#include<iterator>

namespace RangeTraits {
	template<typename, typename = void>
	struct is_iterator : std::false_type {};

	template<typename Iterator>
	struct is_iterator<Iterator, std::void_t<std::enable_if_t<
		!std::is_same_v<typename std::iterator_traits<Iterator>::value_type, void>>>> : std::true_type{};

	template<typename... Iterators>
	struct are_all_iterators {
		static constexpr bool value = (... && RangeTraits::is_iterator<Iterators>::value);
	};

	template<typename Range>
	constexpr bool isRange() {
		auto has_begin_detector = [](auto&& range)->decltype(range.begin()) {};
		if constexpr (!typeInformation::template is_valid<Range>(has_begin_detector))
			return false;
		if constexpr (!is_iterator<typename typeInformation::get_expression_type_if_valid<
			decltype(has_begin_detector), Range >::type>::value)
			return false;

		auto has_end_detector = [](auto&& range)->decltype(range.end()) {};
		if constexpr (!typeInformation::template is_valid<Range>(has_end_detector))
			return false;
		if constexpr (!is_iterator <typename typeInformation::get_expression_type_if_valid<
			decltype(has_end_detector), Range >::type >::value)
			return false;

		return true;
	}

	template<typename... Ranges>
	struct are_all_ranges {
		static constexpr bool value = (... && isRange<Ranges>());
	};
}
namespace ranges {
	namespace internals {
		namespace iterators {
			template<typename... Iterators>
			class cartezian_product_iterator {
				static_assert(RangeTraits::are_all_iterators<Iterators...>::value,
					"error: all template parameters should be iterators");

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

				auto getEndIterator() const {
					auto copyOfThis = *this;
					copyOfThis.setAllCurrentIteratorsToTheirEnd(
						std::make_index_sequence<std::tuple_size_v<decltype(copyOfThis.m_beginCurrenEndIterTuples)>>{});
					return copyOfThis;
				}

			private:
				template<std::size_t... indicies>
				void setAllCurrentIteratorsToTheirEnd(std::index_sequence<indicies...>) {
					auto setCurrentIteratorsToEnd = [](auto& iterTuple) {
						std::get<1>(iterTuple) = std::get<2>(iterTuple);
					};
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
#include<vector>
#include<algorithm>

namespace ranges {
	namespace view {
		template<typename... Ranges>
		class cartezian_product_range_adapter {
			static_assert(RangeTraits::are_all_ranges<Ranges...>::value,
				"Error: all types in the given list of types must be Ranges");
			std::tuple<Ranges...> m_ranges;

		public:
			using value_type = std::tuple<typename Ranges::value_type...>;
			using iterator = ranges::internals::iterators::cartezian_product_iterator<typename Ranges::const_iterator...>;

			cartezian_product_range_adapter(const Ranges&... ranges)
				noexcept(noexcept(std::make_tuple(ranges...)))
				:m_ranges(std::make_tuple(ranges...)) {}

			cartezian_product_range_adapter(const cartezian_product_range_adapter<Ranges...>&) = default;
			cartezian_product_range_adapter(cartezian_product_range_adapter<Ranges...>&&) = default;

			auto begin() const {
				return this->extractBeginEndIteratorsFromRangesTupleToCallBegin(
					std::make_index_sequence<std::tuple_size_v<decltype(this->m_ranges)>>{});
			}

			auto end() const {
				return this->begin().getEndIterator();
			}

			decltype(auto) toVector() const {
				std::vector<value_type> result;
				std::copy(this->begin(), this->end(), std::back_inserter(result));
				return result;
			}

			operator std::vector<value_type>() const {
				return this->toVector();
			}

		private:
			template<std::size_t... indicies>
			auto extractBeginEndIteratorsFromRangesTupleToCallBegin(std::index_sequence<indicies...>) const {
				return ranges::internals::iterators::cartezian_product_iterator
				(std::pair(std::get<indicies>(this->m_ranges).cbegin(), std::get<indicies>(this->m_ranges).cend())...);
			}
		};

		template<typename... Ranges>
		auto cartezian_product(Ranges&&... ranges) {
			static_assert(RangeTraits::are_all_ranges<Ranges...>::value,
				"error: all given types must be range types");
			return ranges::view::cartezian_product_range_adapter{ std::forward<Ranges>(ranges)... };
		}
	}
}
namespace ranges {
	namespace internals {
		namespace iterators {
			template<typename Iterator, typename FilterFunc>
			class filter_iterator {
				static_assert(RangeTraits::is_iterator<Iterator>::value,
					"filter_iterator must accept iterator type: the given iterator is not an iterator type");

				Iterator m_current;
				Iterator m_end;
				FilterFunc m_filter;

			public:
				using iterator_category = std::input_iterator_tag;
				using value_type = typename std::iterator_traits<Iterator>::value_type;
				using difference_type = std::ptrdiff_t;
				using pointer = typename std::iterator_traits<Iterator>::pointer;
				using reference = typename std::iterator_traits<Iterator>::reference;

				explicit filter_iterator(Iterator current, Iterator end, FilterFunc filter)
					noexcept(std::is_nothrow_move_constructible_v<Iterator>&& std::is_nothrow_move_assignable_v<FilterFunc>)
					:m_current(std::move(current)), m_end(std::move(end)), m_filter(std::move(filter))
				{
					while (m_current != m_end && !m_filter(*m_current))
						++m_current;
				}

				filter_iterator() = delete;

				filter_iterator(const filter_iterator<Iterator, FilterFunc>&)
					noexcept(std::is_nothrow_copy_constructible_v<Iterator>&& std::is_nothrow_copy_constructible_v<FilterFunc>) = default;

				filter_iterator(filter_iterator<Iterator, FilterFunc>&&)
					noexcept(std::is_nothrow_move_constructible_v<Iterator>&& std::is_nothrow_move_constructible_v<FilterFunc>) = default;

				filter_iterator<Iterator, FilterFunc>& operator=(const filter_iterator<Iterator, FilterFunc>& other)
					noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
				{
					if (this != &other) {
						this->m_current = other.m_current;
						this->m_end = other.m_end;
					}
					return *this;
				}

				filter_iterator<Iterator, FilterFunc>& operator=(filter_iterator<Iterator, FilterFunc>&& other)
					noexcept(std::is_nothrow_move_constructible_v<Iterator>)
				{
					if (this != &other) {
						this->m_current = std::move(other.m_current);
						this->m_end = std::move(other.m_end);
					}
					return *this;
				}

				filter_iterator<Iterator, FilterFunc>& operator++() {
					if (this->m_current == this->m_end)
						return *this;
					do {
						++this->m_current;
					} while (this->m_current != this->m_end && !this->m_filter(*this->m_current));
					return *this;
				}

				filter_iterator<Iterator, FilterFunc> operator++(int) {
					auto former_value = *this;
					++(*this);
					return former_value;
				}

				bool operator==(const filter_iterator<Iterator, FilterFunc>& other) const noexcept(noexcept(this->m_current == other.m_current)) {
					return this->m_current == other.m_current && this->m_end == other.m_end;
				}

				bool operator!=(const filter_iterator<Iterator, FilterFunc>& other) const noexcept(noexcept(*this == other)) {
					return !(*this == other);
				}

				decltype(auto) operator*() const noexcept(noexcept(*m_current)) {
					return *this->m_current;
				}
			};

		}
	}
}

namespace ranges {
	namespace view {
		template<typename Range, typename FilterFunc>
		class filter_range_adapter {
			static_assert(RangeTraits::isRange<Range>(), "error: the given first type must be a Range type");
			const Range& m_range;
			FilterFunc m_filter;

		public:
			using value_type = typename Range::value_type;
			using iterator = ranges::internals::iterators::filter_iterator<typename Range::iterator, FilterFunc>;

			filter_range_adapter(const Range& range, FilterFunc filter)
				noexcept(std::is_move_constructible_v<FilterFunc>)
				:m_range(range), m_filter(std::move(filter)) {}

			filter_range_adapter(const filter_range_adapter<Range, FilterFunc>& other)
				noexcept(std::is_copy_constructible_v<FilterFunc>)
				:m_range(other.m_range), m_filter(other.m_filter) {}

			filter_range_adapter(filter_range_adapter<Range, FilterFunc>&& other)
				noexcept(std::is_move_constructible_v<FilterFunc>)
				:m_range(other.m_range), m_filter(std::move(other.m_filter)) {}

			auto begin() const {
				return ranges::internals::iterators::filter_iterator(this->m_range.begin(), this->m_range.end(), this->m_filter);
			}

			auto end() const {
				return ranges::internals::iterators::filter_iterator(this->m_range.end(), this->m_range.end(), this->m_filter);
			}

			decltype(auto) toVector() const {
				std::vector<value_type> result;
				std::copy(this->begin(), this->end(), std::back_inserter(result));
				return result;
			}

			operator std::vector<value_type>() const {
				return this->toVector();
			}
		};
	}
}

namespace ranges {
	namespace internals {
		namespace adaptorFactories {
			template<typename FilterFunc>
			class filter_range_adaptor_factory {
				FilterFunc m_filter;
			public:
				filter_range_adaptor_factory(FilterFunc filter)
					noexcept(std::is_nothrow_move_constructible_v<FilterFunc>)
					:m_filter(std::move(filter)) {}

				template<typename Range>
				auto operator()(Range&& range) const {
					static_assert(RangeTraits::isRange<Range>(),
						"error: the given type must be a Range type");
					return ranges::view::filter_range_adapter(std::forward<Range>(range), m_filter);
				}
			};
		}
	}
}

namespace ranges {
	namespace view {
		template<typename FilterFunc>
		auto filter(FilterFunc&& filter) {
			return ranges::internals::adaptorFactories::filter_range_adaptor_factory(
				std::forward<FilterFunc>(filter));
		}
	}
}


template<typename Range, typename FilterFunc>
auto operator|(Range&& range,
	const ranges::internals::adaptorFactories::filter_range_adaptor_factory<FilterFunc>& filterRangeFactory) {
	static_assert(RangeTraits::isRange<Range>(),
		"error: the given first type must be a Range type");
	return filterRangeFactory(std::forward<Range>(range));
}

namespace ranges {
	namespace internals {
		namespace iterators {
			template<typename Advancable>
			class generator_iterator {
				static_assert(typeInformation::is_valid<Advancable>([](auto&& val)->decltype(++val) {}),
					"error: given type must be type with operator ++(prefix version)");
				static_assert(std::is_assignable_v<
					std::add_lvalue_reference_t<Advancable>, std::add_lvalue_reference_t<Advancable>>
					, "error: given type must be assignable");

				Advancable m_cur;
			public:
				using iterator_category = std::input_iterator_tag;
				using value_type = Advancable;
				using difference_type = std::ptrdiff_t;
				using pointer = std::add_pointer_t<Advancable>;
				using reference = std::add_lvalue_reference_t<Advancable>;

				explicit generator_iterator(Advancable&& value)
					noexcept(std::is_nothrow_move_constructible_v<Advancable>) : m_cur(std::move(value)) {}

				explicit generator_iterator(const Advancable& value)
					noexcept(std::is_nothrow_copy_constructible_v<Advancable>) : m_cur(value) {}

				generator_iterator() = delete;
				generator_iterator(const generator_iterator<Advancable>&) = default;
				generator_iterator(generator_iterator&&) = default;

				generator_iterator& operator=(const generator_iterator&) = default;
				generator_iterator& operator=(generator_iterator&&) = default;

				generator_iterator& operator++() {
					++this->m_cur;
					return *this;
				}

				generator_iterator operator++(int) {
					auto former_value = *this;
					++(*this);
					return former_value;
				}

				bool operator==(const generator_iterator& other) const {
					return this->m_cur == other.m_cur;
				}

				bool operator!=(const generator_iterator& other) const {
					return !(*this == other);
				}

				auto operator*() const {
					return this->m_cur;
				}
			};
		}
	}
}
#include<numeric>
#include<limits>

namespace ranges {
	namespace view {
		template<typename Advancable>
		class generator_range_adapter {
			Advancable m_initialValue;
			ranges::internals::iterators::generator_iterator<Advancable> endIter;

		public:
			using value_type = Advancable;
			using iterator = ranges::internals::iterators::generator_iterator<Advancable>;

			generator_range_adapter(const Advancable& initalValue, const Advancable& infinityValue)
				:m_initialValue(initalValue), endIter(infinityValue) {}

			generator_range_adapter(const generator_range_adapter<Advancable>&) = default;
			generator_range_adapter(generator_range_adapter<Advancable>&&) = default;

			auto begin() const noexcept(std::is_nothrow_constructible_v<iterator, Advancable>) {
				return ranges::internals::iterators::generator_iterator{ this->m_initialValue };
			}

			auto end() const noexcept(std::is_nothrow_copy_constructible_v<iterator>) {
				return this->endIter;
			}
		};

		inline auto ints(int initial_value, int end_value = std::numeric_limits<int>::max() / 4096) noexcept {
			return ranges::view::generator_range_adapter<int>(initial_value, end_value);
		}
	}
}

namespace ranges {
	namespace view {
		template<typename Range>
		class take_range_adapter {
			static_assert(RangeTraits::isRange<Range>(), "error: the given type must be a range type");
			const Range& m_range;
			int m_limit;
			typename Range::const_iterator endIter;
		public:
			using value_type = typename Range::value_type;
			using iterator = typename Range::const_iterator;

			explicit take_range_adapter(const Range& range, int limit) noexcept : m_range(range), m_limit(limit) {
				auto curIter = this->m_range.begin();
				for (int i = 0; i < m_limit && curIter != this->m_range.end(); i++)
					curIter++;
				this->endIter = curIter;
			}
			take_range_adapter(const take_range_adapter&) = default;
			take_range_adapter(take_range_adapter&&) = default;

			auto begin() const {
				return this->m_range.begin();
			}
			auto end() const {
				return endIter;
			}

			auto toVector() const {
				std::vector<value_type> result;
				std::copy(this->begin(), this->end(), std::back_inserter(result));
				return result;
			}

			operator std::vector<value_type>() const {
				return this->toVector();
			}
		};
	}

	namespace internals {
		namespace adapterFactories {
			class take_range_adapter_factory {
				int m_limit;
			public:
				explicit take_range_adapter_factory(int limit)noexcept :m_limit(limit) {}
				template<typename Range>
				auto operator()(Range&& range) const {
					static_assert(RangeTraits::isRange<Range>(), "error: the given type must be a range type");
					return ranges::view::take_range_adapter(std::forward<Range>(range), m_limit);
				}
			};
		}
	}

	namespace view {
		inline auto take(int limit) noexcept {
			return ranges::internals::adapterFactories::take_range_adapter_factory(limit);
		}
	}
}

template<typename Range>
auto operator|(Range&& range, const ranges::internals::adapterFactories::take_range_adapter_factory& factory) {
	static_assert(RangeTraits::isRange<Range>(), "error: the given type must be a range type");
	return factory(std::forward<Range>(range));
}

namespace ranges {
	namespace internals {
		namespace iterators {
			template<typename Iterator, typename TransformFunc,
				typename Stub = decltype(std::declval<TransformFunc>()(std::declval<typename std::iterator_traits<Iterator>::value_type>()))>
				class transform_iterator
			{
				static_assert(RangeTraits::is_iterator<Iterator>::value,
					"error: the first type must be an iterator type");

				Iterator m_curIterator;
				TransformFunc m_transform;
			public:
				using iterator_category = std::input_iterator_tag;
				using value_type = decltype(std::declval<TransformFunc>()((std::declval<typename std::iterator_traits<Iterator>::value_type>())));
				using difference_type = std::ptrdiff_t;
				using pointer = std::add_pointer_t<value_type>;
				using reference = std::add_lvalue_reference_t<value_type>;

				explicit transform_iterator(Iterator it, TransformFunc func)
					noexcept(std::is_nothrow_move_constructible_v<Iterator>&& std::is_nothrow_move_constructible_v<TransformFunc>) :
					m_curIterator(std::move(it)), m_transform(std::move(func)) {}

				transform_iterator() noexcept = delete;

				transform_iterator(const transform_iterator<Iterator, TransformFunc, Stub>& other)
					noexcept(std::is_nothrow_copy_constructible_v<Iterator>&& std::is_nothrow_copy_constructible_v<TransformFunc>) = default;

				transform_iterator(transform_iterator<Iterator, TransformFunc, Stub>&& other)
					noexcept(std::is_nothrow_move_constructible_v<Iterator>&& std::is_nothrow_move_constructible_v<TransformFunc>) = default;

				transform_iterator<Iterator, TransformFunc, Stub>& operator=(const transform_iterator<Iterator, TransformFunc, Stub>& other)
					noexcept(std::is_nothrow_copy_assignable_v<Iterator>) {
					this->m_curIterator = other.m_curIterator;
					return *this;
				}

				transform_iterator<Iterator, TransformFunc, Stub>& operator=(transform_iterator<Iterator, TransformFunc, Stub>&& other)
					noexcept(std::is_nothrow_move_assignable_v<Iterator>) {
					this->m_curIterator = std::move(other.m_curIterator);
					return *this;
				}

				transform_iterator<Iterator, TransformFunc, Stub>& operator++() noexcept(noexcept(++m_curIterator)) {
					++m_curIterator; return *this;
				}
				transform_iterator<Iterator, TransformFunc, Stub> operator++(int) {
					auto former_iterator = *this;
					++(*this);
					return former_iterator;
				}

				bool operator==(const transform_iterator<Iterator, TransformFunc, Stub>& other)
					const noexcept(noexcept(this->m_curIterator == other.m_curIterator)) {
					return this->m_curIterator == other.m_curIterator;
				}

				bool operator!=(const transform_iterator<Iterator, TransformFunc, Stub>& other)
					const noexcept(noexcept(*this == other)) {
					return !(*this == other);
				}

				decltype(auto) operator*() const noexcept(noexcept(m_transform(*m_curIterator))) {
					return m_transform(*m_curIterator);
				}
			};
		}
	}
}

namespace ranges {
	namespace view {
		template<typename Range, typename TransformFunc>
		class transform_range_adaptor {
			static_assert(RangeTraits::isRange<Range>(),
				"error: the first type must be a Range type");

			const Range& m_range;
			TransformFunc m_transform;

			//this static variables remove code duplication in the noexcept clauses
			static constexpr bool is_transformFunc_has_nothrow_move_ctor = std::is_nothrow_move_constructible_v<TransformFunc>;
			static constexpr bool is_transformFunc_has_nothrow_copy_ctor = std::is_nothrow_copy_constructible_v<TransformFunc>;
		public:
			using value_type = std::invoke_result_t<TransformFunc, typename Range::value_type>;
			using iterator = ranges::internals::iterators::transform_iterator<typename Range::iterator, TransformFunc>;

			transform_range_adaptor(const Range& range, TransformFunc func)
				noexcept(is_transformFunc_has_nothrow_move_ctor)
				: m_transform(std::move(func)), m_range(range) {}

			transform_range_adaptor(const transform_range_adaptor<Range, TransformFunc>& other)
				noexcept(is_transformFunc_has_nothrow_copy_ctor)
				:m_transform(other.m_transform), m_range(other.m_range) {}

			transform_range_adaptor(transform_range_adaptor<Range, TransformFunc>&& other)
				noexcept(is_transformFunc_has_nothrow_move_ctor)
				:m_transform(std::move(other.m_transform)), m_range(other.m_range) {}

			auto begin() const {
				return ranges::internals::iterators::transform_iterator(this->m_range.begin(), this->m_transform);
			}
			auto end() const {
				return ranges::internals::iterators::transform_iterator(this->m_range.end(), this->m_transform);
			}

			decltype(auto) toVector() const {
				std::vector<value_type> result;
				std::copy(this->begin(), this->end(), std::back_inserter(result));
				return result;
			}

			operator std::vector<value_type>() const {
				return this->toVector();
			}
		};
	}
}

namespace ranges {
	namespace internals {
		namespace adaptorFactories {
			template<typename TransformFunc>
			class transform_range_adaptor_factory {
				TransformFunc m_transform;
			public:
				transform_range_adaptor_factory(TransformFunc transform)
					noexcept(noexcept(std::is_nothrow_move_constructible_v<TransformFunc>))
					:m_transform(std::move(transform)) {}

				template<typename Range>
				auto operator()(Range&& range) const {
					static_assert(RangeTraits::isRange<Range>(),
						"error: the given type must be a range type");
					return ranges::view::transform_range_adaptor(std::forward<Range>(range), m_transform);
				}
			};
		}
	}
}

namespace ranges {
	namespace view {
		template<typename TransformFunc>
		auto transform(TransformFunc&& transform) {
			return ranges::internals::adaptorFactories::transform_range_adaptor_factory(
				std::forward<TransformFunc>(transform));
		}
	}
}

template<typename Range, typename TransformFunc>
auto operator|(Range&& range,
	const ranges::internals::adaptorFactories::transform_range_adaptor_factory<TransformFunc>& transformRangeFactory) {
	static_assert(RangeTraits::isRange<Range>(),
		"error: the given first type must be a Range type");
	return transformRangeFactory(std::forward<Range>(range));
}

namespace ranges {
	namespace internals {
		namespace iterators {

			template<typename FirstIterator, typename SecondIterator>
			class zip_iterator {
				static_assert(RangeTraits::is_iterator<FirstIterator>::value
					&& RangeTraits::is_iterator<SecondIterator>::value,
					"the given two types must be iterators");

				FirstIterator m_firstIter;
				SecondIterator m_secondIter;

			public:
				using iterator_category = std::input_iterator_tag;
				using value_type = std::pair<typename FirstIterator::value_type,
					typename SecondIterator::value_type>;
				using difference_type = std::ptrdiff_t;
				using pointer = std::add_pointer_t<value_type>;
				using reference = std::add_lvalue_reference_t<value_type>;

				zip_iterator(FirstIterator firstIter, SecondIterator secondIter)
					noexcept(std::is_move_constructible_v<FirstIterator>&& std::is_move_constructible_v<SecondIterator>)
					:m_firstIter(std::move(firstIter)), m_secondIter(std::move(secondIter)) {}

				zip_iterator() = delete;

				zip_iterator(const zip_iterator<FirstIterator, SecondIterator>&) = default;
				zip_iterator(zip_iterator<FirstIterator, SecondIterator>&&) = default;

				zip_iterator<FirstIterator, SecondIterator>& operator=(const zip_iterator<FirstIterator, SecondIterator>&) = default;
				zip_iterator<FirstIterator, SecondIterator>& operator=(zip_iterator<FirstIterator, SecondIterator>&&) = default;

				zip_iterator<FirstIterator, SecondIterator>& operator++()
					noexcept(noexcept(++this->m_firstIter) && noexcept(++this->m_secondIter))
				{
					++this->m_firstIter;
					++this->m_secondIter;
					return *this;
				}

				zip_iterator<FirstIterator, SecondIterator> operator++(int) {
					auto former_value = *this;
					++(*this);
					return former_value;
				}

				bool operator==(const zip_iterator<FirstIterator, SecondIterator>& other) const
					noexcept(noexcept(this->m_firstIter == this->m_firstIter) && noexcept(this->m_secondIter == this->m_secondIter))
				{
					return this->m_firstIter == other.m_firstIter && this->m_secondIter == other.m_secondIter;
				}

				bool operator!=(const zip_iterator<FirstIterator, SecondIterator>& other) const
					noexcept(noexcept(*this == other))
				{
					return !(*this == other);
				}

				decltype(auto) operator*() const {
					return std::pair(*(this->m_firstIter), *(this->m_secondIter));
				}
			};
		}
	}
}

namespace ranges {
	namespace view {

		template<typename FirstRange, typename SecondRange>
		class zip_range_adapter {
			static_assert(RangeTraits::isRange<FirstRange>() && RangeTraits::isRange<SecondRange>(),
				"error: the given two types must be Range types");

			const FirstRange& m_firstRange;
			const SecondRange& m_secondRange;

		public:
			using value_type = std::pair<typename FirstRange::value_type, typename SecondRange::value_type>;
			using iterator = ranges::internals::iterators::zip_iterator<typename FirstRange::iterator, typename SecondRange::iterator>;

			zip_range_adapter(const FirstRange& firstRange, const SecondRange& secondRange) noexcept
				: m_firstRange(firstRange), m_secondRange(secondRange) {}
			zip_range_adapter(const zip_range_adapter<FirstRange, SecondRange>& other) noexcept
				: m_firstRange(other.m_firstRange), m_secondRange(other.m_secondRange) {}

			auto begin() const
				noexcept(std::is_nothrow_constructible_v<ranges::internals::iterators::zip_iterator<
					typename FirstRange::iterator, typename SecondRange::iterator>, typename FirstRange::iterator, typename SecondRange::iterator >)
			{
				return ranges::internals::iterators::zip_iterator(this->m_firstRange.begin(), this->m_secondRange.begin());
			}

			auto end() const
				noexcept(std::is_nothrow_constructible_v<ranges::internals::iterators::zip_iterator<
					typename FirstRange::iterator, typename SecondRange::iterator>, typename FirstRange::iterator, typename SecondRange::iterator >)
			{
				return ranges::internals::iterators::zip_iterator(this->m_firstRange.end(), this->m_secondRange.end());
			}

			decltype(auto) toVector() const {
				std::vector<value_type> result;
				std::copy(this->begin(), this->end(), std::back_inserter(result));
				return result;
			}

			operator std::vector<value_type>() const {
				return this->toVector();
			}
		};
	}
}

namespace ranges {
	namespace view {
		template<typename FirstRange, typename SecondRange>
		auto zip(FirstRange&& firstRange, SecondRange&& secondRange) {
			static_assert(RangeTraits::isRange<FirstRange>() && RangeTraits::isRange<SecondRange>(),
				"error: the given two types must be Range types");

			return ranges::view::zip_range_adapter(firstRange, secondRange);
		}
	}
}
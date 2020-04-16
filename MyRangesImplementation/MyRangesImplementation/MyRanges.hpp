#pragma once
#include<iterator>
#include<type_traits>
#include<optional>
#include<vector>
#include<variant>

template<typename... Handlers>
struct overload : public Handlers...{
	using Handlers::operator()...;
};

template<typename... Handlers>
overload(Handlers...)->overload<Handlers...>;

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
	constexpr bool is_valid(Func&& f) {
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

namespace RangeTraits {
	template<typename, typename = void>
	struct is_iterator : std::false_type {};

	template<typename Iterator>
	struct is_iterator<Iterator, std::void_t<std::enable_if_t<
		!std::is_same_v<typename std::iterator_traits<Iterator>::value_type, void>>>> : std::true_type{};

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
}


namespace ranges {
	namespace internals {
		namespace iterators {
			template<typename Iterator, typename TransformFunc,
				typename Stub = decltype(std::declval<TransformFunc>()(std::declval<typename std::iterator_traits<Iterator>::value_type>()))>
				class transform_iterator
			{
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

			template<typename Iterator, typename FilterFunc, 
				typename Stub = std::enable_if_t<RangeTraits::is_iterator<Iterator>::value>>
				class filter_iterator {
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

					filter_iterator(const filter_iterator<Iterator, FilterFunc, Stub>&)
						noexcept(std::is_nothrow_copy_constructible_v<Iterator>&& std::is_nothrow_copy_constructible_v<FilterFunc>) = default;

					filter_iterator(filter_iterator<Iterator, FilterFunc, Stub>&&)
						noexcept(std::is_nothrow_move_constructible_v<Iterator>&& std::is_nothrow_move_constructible_v<FilterFunc>) = default;

					filter_iterator<Iterator, FilterFunc, Stub>& operator=(const filter_iterator<Iterator, FilterFunc, Stub>& other)
						noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
					{
						if (this != &other) {
							this->m_current = other.m_current;
							this->m_end = other.m_end;
						}
						return *this;
					}

					filter_iterator<Iterator, FilterFunc, Stub>& operator=(filter_iterator<Iterator, FilterFunc, Stub>&& other)
						noexcept(std::is_nothrow_move_constructible_v<Iterator>)
					{
						if (this != &other) {
							this->m_current = std::move(other.m_current);
							this->m_end = std::move(other.m_end);
						}
						return *this;
					}

					filter_iterator<Iterator, FilterFunc, Stub>& operator++(){
						if (this->m_current == this->m_end)
							return *this;
						do {
							++this->m_current;
						} while (this->m_current != this->m_end && !this->m_filter(*this->m_current));
						return *this;
					}

					filter_iterator<Iterator, FilterFunc, Stub> operator++(int) {
						auto former_value = *this;
						++(*this);
						return former_value;
					}

					bool operator==(const filter_iterator<Iterator, FilterFunc, Stub>& other) const noexcept(noexcept(this->m_current == other.m_current)) {
						return this->m_current == other.m_current && this->m_end == other.m_end;
					}

					bool operator!=(const filter_iterator<Iterator, FilterFunc, Stub>& other) const noexcept(noexcept(*this == other)) {
						return !(*this == other);
					}

					decltype(auto) operator*() const noexcept(noexcept(*m_current)) {
						return *this->m_current;
					}
			};

			template<typename FirstIterator, typename SecondIterator,
				typename Stub = std::enable_if_t<RangeTraits::is_iterator<FirstIterator>::value && RangeTraits::is_iterator<SecondIterator>::value, void>>
				class zip_iterator {
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

					zip_iterator(const zip_iterator<FirstIterator, SecondIterator, Stub>&) = default;
					zip_iterator(zip_iterator<FirstIterator, SecondIterator, Stub>&&) = default;

					zip_iterator<FirstIterator, SecondIterator, Stub>& operator=(const zip_iterator<FirstIterator, SecondIterator, Stub>&) = default;
					zip_iterator<FirstIterator, SecondIterator, Stub>& operator=(zip_iterator<FirstIterator, SecondIterator, Stub>&&) = default;

					zip_iterator<FirstIterator, SecondIterator, Stub>& operator++()
						noexcept(noexcept(++this->m_firstIter) && noexcept(++this->m_secondIter))
					{
						++this->m_firstIter;
						++this->m_secondIter;
						return *this;
					}

					zip_iterator<FirstIterator, SecondIterator, Stub> operator++(int) {
						auto former_value = *this;
						++(*this);
						return former_value;
					}

					bool operator==(const zip_iterator<FirstIterator, SecondIterator, Stub>& other) const
						noexcept(noexcept(this->m_firstIter == this->m_firstIter) && noexcept(this->m_secondIter == this->m_secondIter))
					{
						return this->m_firstIter == other.m_firstIter && this->m_secondIter == other.m_secondIter;
					}

					bool operator!=(const zip_iterator<FirstIterator, SecondIterator, Stub>& other) const
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

	namespace view {
		template<typename Range, typename TransformFunc, typename Stub = std::enable_if_t<RangeTraits::isRange<Range>(), void>>
		class transform_range_adaptor {
			const Range& m_range;
			TransformFunc m_transform;

			//this static variables remove code duplication in the noexcept clauses
			static constexpr bool is_transformFunc_has_nothrow_move_ctor = std::is_nothrow_move_constructible_v<TransformFunc>;
			static constexpr bool is_transformFunc_has_nothrow_copy_ctor = std::is_nothrow_copy_constructible_v<TransformFunc>;
		public:
			using value_type = std::invoke_result_t<TransformFunc, typename Range::value_type>;
			using iterator = ranges::internals::iterators::transform_iterator<typename Range::iterator, TransformFunc>;

			transform_range_adaptor(const Range& range, TransformFunc func)
				noexcept(is_transformFunc_has_nothrow_move_ctor )
				: m_transform(std::move(func)), m_range(range) {}

			transform_range_adaptor(const transform_range_adaptor<Range, TransformFunc, Stub>& other)
				noexcept(is_transformFunc_has_nothrow_copy_ctor)
				:m_transform(other.m_transform), m_range(other.m_range) {}

			transform_range_adaptor(transform_range_adaptor<Range, TransformFunc, Stub>&& other)
				noexcept(is_transformFunc_has_nothrow_move_ctor)
				:m_transform(std::move(other.m_transform)), m_range(other.m_range) {}

			auto begin() const{
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

		template<typename Range, typename FilterFunc, typename Stub = std::enable_if_t<RangeTraits::isRange<Range>(), void>>
		class filter_range_adapter {
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

			decltype(auto) toVector() const{
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
		namespace adaptorFactories {
			template<typename TransformFunc>
			class transform_range_adaptor_factory {
				TransformFunc m_transform;
			public:
				transform_range_adaptor_factory(TransformFunc transform)
					noexcept(noexcept(std::is_nothrow_move_constructible_v<TransformFunc>))
					:m_transform(std::move(transform)) {}

				template<typename Range, typename = std::enable_if_t<RangeTraits::isRange<Range>(), void>>
				auto operator()(Range&& range) const {
					return ranges::view::transform_range_adaptor(std::forward<Range>(range), m_transform);
				}
			};

			template<typename FilterFunc>
			class filter_range_adaptor_factory {
				FilterFunc m_filter;
			public:
				filter_range_adaptor_factory(FilterFunc filter)
					noexcept(std::is_nothrow_move_constructible_v<FilterFunc>)
					:m_filter(std::move(filter)) {}

				template<typename Range, typename = std::enable_if_t<RangeTraits::isRange<Range>(), void>>
				auto operator()(Range&& range) const {
					return ranges::view::filter_range_adapter(std::forward<Range>(range), m_filter);
				}
			};
		}
	}

	namespace view {
		template<typename TransformFunc>
		auto transform(TransformFunc&& transform){
			return ranges::internals::adaptorFactories::transform_range_adaptor_factory(
				std::forward<TransformFunc>(transform));
		}

		template<typename FilterFunc>
		auto filter(FilterFunc&& filter) {
			return ranges::internals::adaptorFactories::filter_range_adaptor_factory(
				std::forward<FilterFunc>(filter));
		}
	}
}

template<typename Range, typename TransformFunc, typename = std::enable_if_t<RangeTraits::isRange<Range>(), void>>
auto operator|(Range&& range,
	const ranges::internals::adaptorFactories::transform_range_adaptor_factory<TransformFunc>& transformRangeFactory) {
	return transformRangeFactory(std::forward<Range>(range));
}

template<typename Range, typename FilterFunc, typename = std::enable_if_t<RangeTraits::isRange<Range>(), void>>
auto operator|(Range&& range,
	const ranges::internals::adaptorFactories::filter_range_adaptor_factory<FilterFunc>& filterRangeFactory) {
	return filterRangeFactory(std::forward<Range>(range));
}

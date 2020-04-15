#pragma once
#include<iterator>
#include<type_traits>

namespace typeValidation {
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
}

namespace RangeTraits {
	template<typename, typename = void>
	struct is_iterator : std::false_type {};

	template<typename Iterator>
	struct is_iterator<Iterator, std::void_t<std::enable_if_t<
		!std::is_same_v<typename std::iterator_traits<Iterator>::value_type, void>>>> : std::true_type{};

	template<typename Range>
	constexpr bool isRange() {
		if constexpr (!typeValidation::template is_valid<Range>([](auto&& range)->decltype(range.begin()) {}))
			return false;
		if constexpr (!typeValidation::template is_valid<Range>([](auto&& range)->decltype(range.end()) {}))
			return false;
		return true;
	}
}


namespace ranges {
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
}

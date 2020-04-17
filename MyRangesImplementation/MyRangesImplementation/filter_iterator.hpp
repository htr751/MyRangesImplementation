#pragma once
#include"range_traits.hpp"
#include<type_traits>

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
					noexcept(std::is_nothrow_move_constructible_v<Iterator> && std::is_nothrow_move_constructible_v<FilterFunc>) = default;

				filter_iterator<Iterator, FilterFunc>& operator=(const filter_iterator<Iterator, FilterFunc> & other)
					noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
				{
					if (this != &other) {
						this->m_current = other.m_current;
						this->m_end = other.m_end;
					}
					return *this;
				}

				filter_iterator<Iterator, FilterFunc>& operator=(filter_iterator<Iterator, FilterFunc> && other)
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

				bool operator==(const filter_iterator<Iterator, FilterFunc> & other) const noexcept(noexcept(this->m_current == other.m_current)) {
					return this->m_current == other.m_current && this->m_end == other.m_end;
				}

				bool operator!=(const filter_iterator<Iterator, FilterFunc> & other) const noexcept(noexcept(*this == other)) {
					return !(*this == other);
				}

				decltype(auto) operator*() const noexcept(noexcept(*m_current)) {
					return *this->m_current;
				}
			};
			
		}
	}
}
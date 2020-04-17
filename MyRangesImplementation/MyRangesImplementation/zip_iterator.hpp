#pragma once
#include"range_traits.hpp"
#include<type_traits>

namespace ranges {
	namespace internals {
		namespace iterators {

			template<typename FirstIterator, typename SecondIterator,
				typename Stub = std::enable_if_t<RangeTraits::is_iterator<FirstIterator>::value&& RangeTraits::is_iterator<SecondIterator>::value, void>>
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
}
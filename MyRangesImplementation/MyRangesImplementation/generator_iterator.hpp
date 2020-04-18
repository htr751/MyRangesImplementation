#pragma once
#include"typeInformation.hpp"
#include<iterator>

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
					noexcept(std::is_nothrow_move_constructible_v<Advancable>): m_cur(std::move(value)) {}

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

				bool operator==(const generator_iterator& other) const{
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
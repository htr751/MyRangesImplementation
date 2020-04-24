#pragma once
#include"generator_iterator.hpp"
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
		//creates range that contains almost all the availiable ints, computed in a lazy evaluation fashion
		inline auto ints(int initial_value, int end_value = std::numeric_limits<int>::max() / 4096) noexcept {
			return ranges::view::generator_range_adapter<int>(initial_value, end_value);
		}
	}
}
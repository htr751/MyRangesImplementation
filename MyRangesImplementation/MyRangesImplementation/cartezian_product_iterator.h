#pragma once
#include<type_traits>
#include"range_traits.hpp"

namespace ranges {
	namespace internals {
		namespace iterators {
			template<typename... Iterators>
			class cartezian_product_iterator {
				static_assert(RangeTraits::are_all_iterators<Iterators...>::value,
					"parameters weren't iterators" );
			};
		}
	}
}

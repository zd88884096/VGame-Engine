#pragma once
#include "../common/common.h"
#include "operator.h"
#include "../data/vector_base.h"
#include <variant>


namespace vmath_vector_operator
{
	using namespace vmath_operator;

	template<typename T>
	using ref_selector_t = vmath_template_core::ref_selector_t<T>;

	struct vectorAdd {};
	struct vectorSub {};
	struct vectorMul {};
	struct vectorDiv {};
	
	template<typename Operation_Tp, typename ...Tps>
	VMATH_INLINE auto getOperator(Tps&& ...args)
	{
		return Operator<Operation_Tp, ref_selector_t<Tps>...> (std::forward<ref_selector_t<Tps>>(args)...);
	}
}

namespace vmath_template_core
{
	template<vmath_operator::detail::OperatorType U>
	struct ref_selector<U>
	{
		typedef std::decay_t<U> type;
	};
}

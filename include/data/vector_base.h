#pragma once
#include "../common/common.h"
#include "../core/template_concept.h"

namespace vmath_vector_operator
{
	template<typename Operation_Tp, typename ...Tps>
	VMATH_INLINE auto getOperator(Tps&& ...args);

	struct vectorAdd;
	struct vectorSub;
	struct vectorDiv;
	struct vectorMul;
}

namespace vmath_vector_base 
{

	// CRTP Pattern
	template<typename Derived_Tp>
	class VectorBase
	{
	public:
		typedef
			typename vmath_concept::valid_type vector_base_valid_type;

		VMATH_INLINE Derived_Tp& derived()
		{
			return static_cast<Derived_Tp&>(*this);
		}

		VMATH_INLINE const Derived_Tp& derived() const
		{
			return static_cast<const Derived_Tp&>(*this);
		}

		template<typename Rhs_Derive_Tp>
		VMATH_CONSTEXPR auto operator+(const Rhs_Derive_Tp& rhs)
		{
			return vmath_vector_operator::getOperator<vmath_vector_operator::vectorAdd>(derived(), rhs.derived());
		}

		template<typename Rhs_Derive_Tp>
		VMATH_CONSTEXPR auto operator-(const Rhs_Derive_Tp& rhs)
		{
			return vmath_vector_operator::getOperator<vmath_vector_operator::vectorSub>(derived(), rhs.derived());
		}

		template<typename Rhs_Derive_Tp>
		VMATH_CONSTEXPR auto operator/(const Rhs_Derive_Tp& rhs)
		{
			return vmath_vector_operator::getOperator<vmath_vector_operator::vectorDiv>(derived(), rhs.derived());
		}

		template<typename Rhs_Derive_Tp>
		VMATH_CONSTEXPR auto operator*(const Rhs_Derive_Tp& rhs)
		{
			return vmath_vector_operator::getOperator<vmath_vector_operator::vectorMul>(derived(), rhs.derived());
		}


	};

};
#pragma once
#include "../common/common.h"
#include "template_concept.h"
#include "template_core.h"
#include "../data/vector_base.h"
#include "../data/tuple_container.h"
#include <tuple>
#include <utility>


namespace vmath_operator
{
	template<typename Operation_Tp, typename... Operand_Tps>
	class Operator;


	namespace detail
	{
		template<typename T>
		concept OperatorType = vmath_concept::Valid_Type<typename T::is_operator_type>;
	}

	template<size_t index, detail::OperatorType Operator_Tp >
	VMATH_INLINE VMATH_CONSTEXPR const auto& get(const Operator_Tp& op)
	{
		return std::get<index>(op.m_Operands);
	}

	template<detail::OperatorType Operator_Tp>
	VMATH_INLINE VMATH_CONSTEXPR const auto& get(const Operator_Tp& op)
	{
		return op.m_Operands;
	}

	template<typename Operation_Tp, typename... Operand_Tps>
	class Operator :
		public vmath_vector_base::VectorBase<Operator<Operation_Tp, Operand_Tps...>>
	{
	protected: 
		template<size_t index, detail::OperatorType Operator_Tp> 
		friend VMATH_INLINE VMATH_CONSTEXPR const auto& get(const Operator_Tp& op);

		template<detail::OperatorType Operator_Tp> 
		friend VMATH_INLINE VMATH_CONSTEXPR const auto& get(const Operator_Tp& op);

	private:
		typedef 
		vmath_template_core::typelist<Operand_Tps...> operand_typelist;

		static constexpr size_t typelist_size = vmath_template_core::typelist_size<operand_typelist>;

		//std::tuple<Operand_Tps...> m_Operands;
		vmath_tuple_container::Tuple<Operand_Tps...> m_Operands;

	public:
		typedef typename vmath_concept::valid_type is_operator_type;

		typedef Operation_Tp operation_type;

		template<size_t index>
		using operand_type = 
			typename vmath_template_core::typelist_at_t<operand_typelist, index>;

		typedef operand_type<0> Lhs_Tp;
		typedef operand_type<1> Rhs_Tp;

		Operator(Operand_Tps ..._operands) noexcept
			: m_Operands(_operands...)
		{

		}
	
		// Common Functionality 
		template<typename U = operand_typelist>
		requires (typelist_size > 0)
		const auto& lhs() const
		{
			return m_Operands.get<0>();
		}

		template<typename U = operand_typelist>
		requires (typelist_size > 1)
		const auto& rhs() const
		{
			return m_Operands.get<1>();
		}
	};



};

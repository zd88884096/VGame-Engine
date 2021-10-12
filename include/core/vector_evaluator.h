#pragma once
#include "../common/common.h"
#include "../core/vector_operator.h"
#include "../data/tuple_container.h"
#include "../simd/simd_operation.h"
#include <algorithm>


namespace vmath_concept
{
	namespace vec_op = vmath_vector_operator;

	template<typename T>
	concept Ev_Vector_Type = Vector_Type<std::decay_t<T>>;

	template<typename T>
	concept Ev_VectorAdd_Type = std::is_same_v<typename std::decay_t<T>::operation_type, vec_op::vectorAdd>;

	template<typename T>
	concept Ev_VectorMul_Type = std::is_same_v<typename std::decay_t<T>::operation_type, vec_op::vectorMul>;

	template<typename T>
	concept Ev_VectorSub_Type = std::is_same_v<typename std::decay_t<T>::operation_type, vec_op::vectorSub>;

	template<typename T>
	concept Ev_VectorDiv_Type = std::is_same_v<typename std::decay_t<T>::operation_type, vec_op::vectorDiv>;
}



namespace vmath_vector_evaluator
{
	namespace vec_op = vmath_vector_operator;
	namespace core = vmath_template_core;
	namespace vconcept = vmath_concept;

	class EvaluatorBase
	{
	public:
		template<vconcept:: Simd_Type Src_Tp, vconcept::Simd_Type Dst_Tp>
		VMATH_INLINE void convertPack(Dst_Tp& dst, const Src_Tp& src)
		{
			vmath_avx::cast_impl(dst, src);
		}

		template<vconcept::Sse_Type Src_Tp, vconcept::Sse_Type Dst_Tp>
		VMATH_INLINE void convertPack(Dst_Tp& dst, const Src_Tp& src)
		{
			vmath_sse::cast_impl(dst, src);
		}

	};

	template<typename Expr>
	class Evaluator;

	template<vconcept::Ev_Vector_Type Vector_Tp>
	class Evaluator<Vector_Tp> : public EvaluatorBase
	{
	private:
		Vector_Tp m_Vector;
	public:
		typedef typename std::decay_t<Vector_Tp>::scalar_type scalar_type;
		static constexpr bool vAlign = (std::decay_t<Vector_Tp>::_alignment != 0);
		static constexpr bool is_fixed_size = std::decay_t<Vector_Tp>::_size != 0;
		
		Evaluator(Vector_Tp&& vector) : m_Vector(vector)
		{

		}

		template<vconcept::Simd_Type Pack_Tp>
		Pack_Tp packEval(size_t index)
		{
			if constexpr (vconcept::Sse_Type<Pack_Tp>)
			{
				return vmath_sse::load<vAlign>(m_Vector.data() + index);
			}
			else {
				return vmath_avx2::load<vAlign>(m_Vector.data() + index);
			}
		}

		template<typename Return_Tp = scalar_type>
			requires std::is_convertible_v<scalar_type, const Return_Tp>
		const Return_Tp eval(size_t index)
		{
			return static_cast<Return_Tp>(m_Vector[index]);
		}

		VMATH_CONSTEXPR size_t size() const
		{
			return m_Vector.size();
		}

	};


	template<typename T, typename Derived_Tp>
	class BinaryEvaluator : public EvaluatorBase
	{
	public:
		typedef typename std::decay_t<T>::Lhs_Tp Lhs_Tp;
		typedef typename std::decay_t<T>::Rhs_Tp Rhs_Tp;
	public:
		Evaluator<Lhs_Tp> m_Lhs;
		Evaluator<Rhs_Tp> m_Rhs;
	public:
		typedef typename Evaluator<Lhs_Tp>::scalar_type left_scalar_type;
		typedef typename Evaluator<Rhs_Tp>::scalar_type right_scalar_type;
		typedef core::upcastType<left_scalar_type,right_scalar_type> scalar_type;
		static constexpr bool is_fixed_size = Evaluator<Lhs_Tp>::is_fixed_size && Evaluator<Rhs_Tp>::is_fixed_size;

		Derived_Tp& derived()
		{
			return static_cast<Derived_Tp&>(*this);
		}

		BinaryEvaluator(const T& expr) : m_Lhs(expr.lhs()), m_Rhs(expr.rhs())
		{

		}


		template<bool isAlign, bool isPackEval, bool useAvx>
		void evalTo(scalar_type* _dst)
		{
			size_t index = 0;
			size_t Size = size();
			if constexpr (isPackEval)
			{
				auto calcLeftStride = [&]() constexpr -> size_t
				{
					if constexpr (isPackEval)
					{
						if constexpr (useAvx) return vconcept::avx_max_num_scalar_v<left_scalar_type>;
						else return vconcept::sse_max_num_scalar_v <left_scalar_type>;
					}
					else return 1;
				};

				auto calcRightStride = [&]() constexpr -> size_t
				{
					if constexpr (isPackEval)
					{
						if constexpr (useAvx) return vconcept::avx_max_num_scalar_v<right_scalar_type>;
						else return vconcept::sse_max_num_scalar_v <right_scalar_type>;
					}
					else return 1;
				};

				constexpr size_t stride = std::min(calcLeftStride(), calcRightStride());
				constexpr size_t maxStride = std::max(calcLeftStride(), calcRightStride());
				for (; index + maxStride <= Size; index += stride)
				{
					if constexpr (useAvx)
					{
						auto res = derived().packEval<vconcept::convert_to_avx_t<scalar_type>>(index);
						vmath_avx2::store<isAlign>(_dst + index, res);
					}
					else
					{
						auto res = derived().packEval<vconcept::convert_to_sse_t<scalar_type>>(index);
						vmath_sse::store<isAlign>(_dst + index, res);
					}
				}

				for (; index < Size; ++index)
				{
					_dst[index] = derived().eval<scalar_type>(index);
				}
			}
			else
			{
				for (; index < Size; ++index)
				{
					_dst[index] = derived().eval<scalar_type>(index);
				}
			}
		}
		
		VMATH_CONSTEXPR size_t leftSize() const
		{
			return m_Lhs.size();
		}

		VMATH_CONSTEXPR size_t rightSize() const
		{
			return m_Rhs.size();
		}

		VMATH_CONSTEXPR size_t size() const
		{
			assert(leftSize() == rightSize());
			return m_Lhs.size();
		}

	};
	template<vconcept::Ev_VectorAdd_Type T>
	class Evaluator<T> : public BinaryEvaluator<T, Evaluator<T>>
	{
	public:
		typedef BinaryEvaluator<T, Evaluator<T>> base_type;
		typedef typename base_type::scalar_type scalar_type;

		Evaluator(const T& expr) : base_type(expr)
		{
	
		}

		template<vconcept::Simd_Type Pack_Tp>
		auto packEval(size_t index)
		{
			auto lpack = base_type::m_Lhs.packEval<Pack_Tp>(index);
			auto rpack = base_type::m_Rhs.packEval<Pack_Tp>(index);
			if constexpr (vconcept::is_sse_data_v<Pack_Tp>)
			{
				if constexpr (std::is_integral_v<typename base_type::left_scalar_type> && 
										std::is_integral_v<typename base_type::right_scalar_type>)
				{
					return vmath_sse::add<32>(lpack, rpack);
				}
				else return vmath_sse::add(lpack, rpack);
			}
			else return vmath_avx::add(lpack, rpack);
		}

		template<typename Return_Tp = scalar_type>
			requires std::is_convertible_v<scalar_type, Return_Tp>
		Return_Tp eval(size_t index)
		{
			return base_type::m_Lhs.eval<Return_Tp>(index) + base_type::m_Rhs.eval<Return_Tp>(index);
		}

	};

	template<vconcept::Ev_VectorSub_Type T>
	class Evaluator<T> : public BinaryEvaluator<T, Evaluator<T>>
	{
	public:
		typedef BinaryEvaluator<T, Evaluator<T>> base_type;
		typedef typename base_type::scalar_type scalar_type;
		Evaluator(const T& expr) : base_type(expr)
		{
	
		}

		template<vconcept::Simd_Type Pack_Tp>
		auto packEval(size_t index)
		{
			auto lpack = base_type::m_Lhs.packEval<Pack_Tp>(index);
			auto rpack = base_type::m_Rhs.packEval<Pack_Tp>(index);
			if constexpr (vconcept::is_sse_data_v<Pack_Tp>) return vmath_sse::sub(lpack, rpack);
			else return vmath_avx::sub(lpack, rpack);
		}

		template<typename Return_Tp = scalar_type>
			requires std::is_convertible_v<scalar_type, Return_Tp>
		Return_Tp eval(size_t index)
		{
			return base_type::m_Lhs.eval<Return_Tp>(index) - base_type::m_Rhs.eval<Return_Tp>(index);
		}

	};
	template<vconcept::Ev_VectorMul_Type T>
	class Evaluator<T> : public BinaryEvaluator<T, Evaluator<T>>
	{
	public:
		typedef BinaryEvaluator<T, Evaluator<T>> base_type;
		typedef typename base_type::scalar_type scalar_type;
		Evaluator(const T& expr) : base_type(expr)
		{
	
		}

		template<vconcept::Simd_Type Pack_Tp>
		auto packEval(size_t index)
		{
			auto lpack = base_type::m_Lhs.packEval<Pack_Tp>(index);
			auto rpack = base_type::m_Rhs.packEval<Pack_Tp>(index);
			if constexpr (vconcept::is_sse_data_v<Pack_Tp>) return vmath_sse::mul(lpack, rpack);
			else return vmath_avx::mul(lpack, rpack);
		}

		template<typename Return_Tp = scalar_type>
			requires std::is_convertible_v<scalar_type, Return_Tp>
		Return_Tp eval(size_t index)
		{
			return base_type::m_Lhs.eval<Return_Tp>(index) * base_type::m_Rhs.eval<Return_Tp>(index);
		}

	};

	template<vconcept::Ev_VectorDiv_Type T>
	class Evaluator<T> : public BinaryEvaluator<T, Evaluator<T>>
	{
	public:
		typedef BinaryEvaluator<T, Evaluator<T>> base_type;
		typedef typename base_type::scalar_type scalar_type;
		Evaluator(const T& expr) : base_type(expr)
		{
	
		}

		template<vconcept::Simd_Type Pack_Tp>
		auto packEval(size_t index)
		{
			auto lpack = base_type::m_Lhs.packEval<Pack_Tp>(index);
			auto rpack = base_type::m_Rhs.packEval<Pack_Tp>(index);
			if constexpr (vconcept::is_sse_data_v<Pack_Tp>) return vmath_sse::div(lpack, rpack);
			else return vmath_avx::div(lpack, rpack);
		}

		template<typename Return_Tp = scalar_type>
			requires std::is_convertible_v<scalar_type, Return_Tp>
		Return_Tp eval(size_t index)
		{
			return base_type::m_Lhs.eval<Return_Tp>(index) / base_type::m_Rhs.eval<Return_Tp>(index);
		}

	};

}
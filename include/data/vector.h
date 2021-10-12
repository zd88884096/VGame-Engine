#include "../common/common.h"
#include "../core/template_core.h"
#include "../simd/simd_arithmetic.h"
#include "../core/operator.h"
#include "../../test/utility/timer.h"
#include "vector_base.h"
#include "../core/vector_operator.h"
#include "vector_container.h"

#include <type_traits>
#include <algorithm>
#include <iostream>



namespace vmath
{
	namespace core = vmath_template_core;
	namespace vconcept = vmath_concept;


	template<size_t _alignment>
	requires vmath_concept::isGreaterEqual<size_t, _alignment, 0>
		struct vectorAlignType
	{
		static constexpr size_t value = _alignment;
	};

	using vectorNoAlignType = vectorAlignType<0>;

	template<typename _Tp1, typename _Tp2>
	constexpr size_t common_alignment_v = std::max(_Tp1::value, _Tp2::value);

	template<typename T, size_t Size, typename _Align_Tp = vectorNoAlignType>
	requires vmath_concept::isGreaterEqual<size_t, Size, 0>
		class Vector;



	namespace detail
	{
		template<size_t _size>
		struct __isDynamicAllocHelper
		{
			static constexpr bool value = _size == DYNAMIC_SIZE;
		};

		template<size_t _size>
		bool constexpr is_dynamic_alloc_v = __isDynamicAllocHelper<_size>::value;




		//-------------------------------------------------------------------------
		//
		//               Implementation of Vector Return Type Helper 
		//
		//-------------------------------------------------------------------------
		template<typename _Tp1, typename  _Tp2>
		struct __VectorReturnTypeHelper
			: vmath_template_core::ReturnTypeHelperBase
		{

			typedef typename _Tp1::scalar_type T;
			typedef typename _Tp2::scalar_type U;
			typedef typename _Tp1::align_type lhs_align_type;
			typedef typename _Tp2::align_type rhs_align_type;
			typedef vmath_template_core::upcastType<T, U> res_scalar_type;
			typedef vectorAlignType<common_alignment_v<lhs_align_type, rhs_align_type>> res_align_type;
			typedef Vector<res_scalar_type, _Tp1::_size, res_align_type>  res_sameAlloc_type;
			typedef Vector<res_scalar_type, DYNAMIC_SIZE, res_align_type>  res_dynamicAlloc_type;

			// Just for Now
			// Need more strategy / condition to determine
			// return allocation type
			FUTURE_CHANGE_MARK
				typedef typename std::conditional<_Tp1::_size == _Tp2::_size,
				res_sameAlloc_type, res_dynamicAlloc_type>::type return_type;
		};

		template<typename _Tp1, typename _Tp2>
		struct returnType_impl {
			typedef vmath_template_core::EmptyType return_type;
		};

		template< typename T, size_t _size1, typename Lhs_Align_Type
			, typename U, size_t _size2, typename Rhs_Align_Type>
			struct returnType_impl<Vector<T, _size1, Lhs_Align_Type>, Vector<U, _size2, Rhs_Align_Type>>
		{
			typedef Vector<T, _size1, Lhs_Align_Type> _Tp1;
			typedef Vector<U, _size2, Rhs_Align_Type> _Tp2;
			typedef typename __VectorReturnTypeHelper<_Tp1, _Tp2>::return_type return_type;
		};

		template<typename _Tp1, typename _Tp2>
		using returnType = typename returnType_impl<_Tp1, _Tp2>::return_type;


	}

	template<typename T, size_t Size, typename _Align_Tp>
		requires vmath_concept::isGreaterEqual<size_t, Size, 0>
	class Vector :
		public vmath_vector_base::VectorBase<Vector<T, Size, _Align_Tp>>
	{
	public:
		// Common type / constant definitions 
		typedef T scalar_type;
		typedef _Align_Tp align_type;
		static constexpr size_t _size = Size;
		static constexpr size_t _alignment = _Align_Tp::value;
		static constexpr bool is_align_v = (_alignment != 0);
	public:
		//-------------------------------------------------------------------------
		//
		//
		//
		//                    Implementation of Vector class
		//
		//
		//
		//-------------------------------------------------------------------------
		vmath_vector_container::vectorContainer<scalar_type, _size> m_Data;


	public:
		typedef vmath_concept::valid_type is_vector_type;
		using vmath_vector_base::VectorBase<Vector<T, Size, _Align_Tp>>::operator=;
		using vmath_vector_base::VectorBase<Vector<T, Size, _Align_Tp>>::operator+;
		using vmath_vector_base::VectorBase<Vector<T, Size, _Align_Tp>>::operator*;
		using vmath_vector_base::VectorBase<Vector<T, Size, _Align_Tp>>::operator-;
		using vmath_vector_base::VectorBase<Vector<T, Size, _Align_Tp>>::operator/;

	

		template<typename U>
		requires std::is_arithmetic_v<U>
			VMATH_API VMATH_CONSTEXPR
			Vector operator*(U _scalar)
		{
			Vector res(*this);

			size_t vectorSize = this->size();
			size_t index = 0;
			return res;
		}

	public:
		template<typename U = T>
		requires (!detail::is_dynamic_alloc_v<_size>)
		VMATH_API Vector()
		{
		
		}

		template<typename U = T>
		requires detail::is_dynamic_alloc_v<_size>
		VMATH_API Vector(size_t _Size) : m_Data(_Size)
		{

		}

		template<vconcept::Vector_Type Rhs_Vector_Type>
		VMATH_API Vector(const Rhs_Vector_Type& rhs) : m_Data(rhs.m_Data)
		{
		
		}

		template<typename Operator_Tp>
		VMATH_API Vector(const Operator_Tp& rhs)
		{
			constexpr bool isPackEvalEnabled = VMATH_ENABLE_SIMD;
			constexpr bool isUseAvx = isDynamic() || (isFixed() && _size >= 8);
			Evaluator<std::decay_t<decltype(rhs)>>(rhs).evalTo<is_align_v, isPackEvalEnabled, isUseAvx> (m_Data.data());
		}

		template<typename Operator_Tp>
			requires (!Operator_Tp::is_fixed_size)
		VMATH_API Vector(const Operator_Tp& rhs) : m_Data(rhs.size())
		{
			constexpr bool isPackEvalEnabled = VMATH_ENABLE_SIMD;
			constexpr bool isUseAvx = isDynamic() || (isFixed() && _size >= 8);
			Evaluator<std::decay_t<decltype(rhs)>>(rhs).evalTo<is_align_v, isPackEvalEnabled, isUseAvx> (m_Data.data());
		}


		template<vconcept::Vector_Type Rhs_Vector_Tp>
		Vector& operator=(const Rhs_Vector_Tp& rhs)
		{
			m_Data = rhs.m_Data;
			return *this;
		}

		template<typename Operator_Tp>
		Vector& operator=(const Operator_Tp& rhs)
		{
			constexpr bool isPackEvalEnabled = VMATH_ENABLE_SIMD;
			constexpr bool isUseAvx = isDynamic() || (isFixed() && _size >= 8);
			Evaluator<std::decay_t<decltype(rhs)>>(rhs).evalTo<is_align_v, isPackEvalEnabled, isUseAvx> (m_Data.data());
			return *this;
		}

	
		VMATH_API ~Vector() = default;

		VMATH_API VMATH_CONSTEXPR T* data()
		{
			return m_Data.data();
		}

		VMATH_API VMATH_CONSTEXPR const T* data() const
		{
			return m_Data.data();
		}

		VMATH_API VMATH_CONSTEXPR size_t size() const
		{
			return m_Data.size();
		}

		VMATH_API VMATH_CONSTEXPR static bool isDynamic()
		{
			return detail::is_dynamic_alloc_v<_size>;
		}

		VMATH_API VMATH_CONSTEXPR static bool isFixed()
		{
			return !detail::is_dynamic_alloc_v<_size>;
		}

		VMATH_API VMATH_CONSTEXPR T& operator[](size_t index)
		{
			return m_Data[index];
		}

		VMATH_API VMATH_CONSTEXPR const T& operator[](size_t index) const
		{
			return m_Data[index];
		}

		template<typename Return_Type = float>
		VMATH_API VMATH_CONSTEXPR Return_Type magnitude() const;

		VMATH_API VMATH_CONSTEXPR Vector normal() const;

		VMATH_API VMATH_CONSTEXPR static Vector zeros();
	};

	template<typename U, typename Rhs_Vector_Tp>
	requires std::is_arithmetic_v<U>
		Rhs_Vector_Tp operator*(U _scalar, const Rhs_Vector_Tp& rhs)
	{
		Vector res(rhs);
		size_t _Size = rhs.size();
		for (int i = 0; i < _Size; ++i)
		{
			res[i] = _scalar * res[i];
		}
		return res;
	}



};


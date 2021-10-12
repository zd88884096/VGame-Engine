#pragma once
#include "../common/common.h"
#include "../core/template_concept.h"
#include <intrin.h>

namespace vmath_concept
{

	template<typename T>
	struct is_sse_data
	{
		static constexpr bool value =
			std::is_same_v<T, __m128> ||
			std::is_same_v<T, __m128d> ||
			std::is_same_v<T, __m128i>;
	};

	template<typename T>
	constexpr bool is_sse_data_v = is_sse_data<T>::value;

	template<typename T>
	concept Sse_Type = is_sse_data_v<T>;

	template<typename T>
	concept Sse_Int_Type = std::is_same_v<T, __m128i>;

	template<typename T>
	concept Sse_Float_Type = std::is_same_v<T, __m128> || std::is_same_v<T, __m128d>;

	namespace detail
	{
		template<typename T, typename U>
		struct is_sse_compatible
		{
			static constexpr bool value = false;
		};

		template<>
		struct is_sse_compatible<float, __m128>
		{
			static constexpr bool value = true;
		};

		template<>
		struct is_sse_compatible<double, __m128d>
		{
			static constexpr bool value = true;
		};

		template<Int_Type T>
		struct is_sse_compatible<T, __m128i>
		{
			static constexpr bool value = true;
		};
	}
	template<typename T, typename U>
	constexpr bool is_sse_compatible_v = detail::is_sse_compatible<T, U>::value;

	namespace detail
	{
		template<typename T>
		struct convert_to_sse;

		template<>
		struct convert_to_sse<double>
		{
			typedef __m128d type;
		};

		template<>
		struct convert_to_sse<float>
		{
			typedef __m128 type;
		};

		template<Int_Type T>
		struct convert_to_sse<T>
		{
			typedef __m128i type;
		};
	}

	template<typename T>
	using convert_to_sse_t = typename detail::convert_to_sse<T>::type;

	template<typename T>
	constexpr size_t sse_max_num_scalar_v = 128 / sizeof(T) / 8;

}

namespace vmath_sse
{
	namespace vconcept = vmath_concept;

	/*
		Float Types	
	*/
	template<bool IsAlign>
	VMATH_INLINE __m128 load(const float* _Src)
	{
		if constexpr (IsAlign)
		{
			return _mm_load_ps(_Src);
		}
		else
		{
			return _mm_loadu_ps(_Src);
		}
	}


	/*
		Double Types	
	*/
	template<bool IsAlign>
	VMATH_INLINE __m128d load(const double* _Src)
	{
		if constexpr (IsAlign)
		{
			return _mm_load_pd(_Src);
		}
		else
		{
			return _mm_loadu_pd(_Src);
		}

	}

	/*
		Integral Types	
	*/

	template<bool IsAlign, vconcept::Int_Type T>
	VMATH_INLINE __m128i load(const T* _Src)
	{
		if constexpr (IsAlign) {
			return _mm_load_si128(reinterpret_cast<const __m128i*>(_Src));
		}
		else {
			return _mm_loadu_si128(reinterpret_cast<const __m128i*>(_Src));
		}
	}

	template<bool IsAlign>
	VMATH_INLINE void store(float* _Dst, __m128 _Src);

	template<>
	VMATH_INLINE void store<false>(float* _Dst, __m128 _Src)
	{
		_mm_storeu_ps(_Dst, _Src);
	}

	template<>
	VMATH_INLINE void store<true>(float* _Dst, __m128 _Src)
	{
		_mm_store_ps(_Dst, _Src);
	}

	template<bool IsAlign>
	VMATH_INLINE void store(double* _Dst, __m128d _Src)
	{
		if constexpr (IsAlign)
		{
			_mm_store_pd(_Dst, _Src);
		}
		else
		{
			_mm_storeu_pd(_Dst, _Src);
		}
	}

	template<bool IsAlign, typename T>
	struct __IntegerStoreHelper
	{

	};

	template<typename U>
	struct __IntegerStoreHelper<false, U>
	{
		VMATH_INLINE static void store(U* _Dst, __m128i _Src)
		{
			_mm_storeu_si128(reinterpret_cast<__m128i*>(_Dst), _Src);
		}
	};

	template<typename U>
	struct __IntegerStoreHelper<true, U>
	{
		VMATH_INLINE static void store(U* _Dst, __m128i _Src)
		{
			_mm_store_si128(reinterpret_cast<__m128i*>(_Dst), _Src);
		}
	};

	template<bool IsAlign, typename T>
	VMATH_INLINE void store(T* _Dst, __m128i _Src)
	{
		__IntegerStoreHelper<IsAlign, T>::store(_Dst, _Src);
	}


	VMATH_INLINE __m128 maskLoad(const float* _Src, int _mask)
	{
		__m128i mask = _mm_set_epi32(_mask << 31, _mask << 30, _mask << 29, _mask << 28);
		return _mm_maskload_ps(_Src, mask);
	}

	VMATH_INLINE __m128d maskLoad(const double* _Src, int _mask)
	{
		__m128i mask = _mm_set_epi64x((_mask & 1) << 63, (_mask & 2) << 62);
		return _mm_maskload_pd(_Src, mask);
	}

	VMATH_INLINE __m128i maskLoad(const int32_t* _Src, int _mask)
	{
		__m128i mask = _mm_set_epi32((_mask & 1) << 31, (_mask & 2) << 30, (_mask & 4) << 29, (_mask & 8) << 28);
		return _mm_maskload_epi32(_Src, mask);
	}

	VMATH_INLINE __m128i maskLoad(const int64_t* _Src, int _mask)
	{
		__m128i mask = _mm_set_epi64x((_mask & 1) << 63, (_mask & 2) << 62);
		return _mm_maskload_epi64(_Src, mask);
	}

	/*
		Casting Functions Implementation	
	*/

	template<vmath_concept::Sse_Type T, vmath_concept::Sse_Type U, 
			vmath_concept::Int_Type Int_Src_Tp = vmath_concept::EmptyType, vmath_concept::Int_Type Int_Dst_Tp = vmath_concept::EmptyType>
	VMATH_INLINE void cast_impl(T& _Dst, const U& _Src);

	template<>
	VMATH_INLINE void cast_impl(__m128& _dst, const __m128i& _src)
	{
		_dst = _mm_cvtepi32_ps(_src);
	}

	template<>
	VMATH_INLINE void cast_impl(__m128d& _dst, const __m128i& _src)
	{
		_dst = _mm_cvtepi32_pd(_src);
	}

	template<>
	VMATH_INLINE void cast_impl(__m128d& _dst, const __m128& _src)
	{
		_dst = _mm_cvtps_pd(_src);
	}

	template<>
	VMATH_INLINE void cast_impl(__m128& _dst, const __m128d& _src)
	{
		_dst = _mm_cvtpd_ps(_src);
	}

	template<>
	VMATH_INLINE void cast_impl(__m128i& _dst, const __m128d& _src)
	{
		_dst = _mm_cvtpd_epi32(_src);
	}

	template<>
	VMATH_INLINE void cast_impl(__m128i& _dst, const __m128& _src)
	{
		_dst = _mm_cvtps_epi32(_src);
	}


	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, int8_t, int16_t>(__m128i& _dst, const __m128i& _src)
	{
		_dst = _mm_cvtepi8_epi16(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, int8_t, int32_t>(__m128i& _dst, const __m128i& _src)
	{
		_dst = _mm_cvtepi8_epi32(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, int8_t, int64_t>(__m128i& _dst, const __m128i& _src)
	{
		_dst = _mm_cvtepi8_epi32(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, int16_t, int32_t>(__m128i& _dst, const __m128i& _src)
	{
		_dst = _mm_cvtepi16_epi32(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, int16_t, int64_t>(__m128i& _dst, const __m128i& _src)
	{
		_dst = _mm_cvtepi16_epi64(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, int32_t, int64_t>(__m128i& _dst, const __m128i& _src)
	{
		_dst = _mm_cvtepi32_epi64(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, uint8_t, int16_t>(__m128i& _dst, const __m128i& _src)
	{
		_dst = _mm_cvtepu8_epi16(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, uint8_t, int32_t>(__m128i& _dst, const __m128i& _src)
	{
		_dst = _mm_cvtepu8_epi32(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, uint8_t, int64_t>(__m128i& _dst, const __m128i& _src)
	{
		_dst = _mm_cvtepu8_epi32(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, uint16_t, int32_t>(__m128i& _dst, const __m128i& _src)
	{
		_dst = _mm_cvtepu16_epi32(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, uint16_t, int64_t>(__m128i& _dst, const __m128i& _src)
	{
		_dst = _mm_cvtepu16_epi64(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, uint32_t, int64_t>(__m128i& _dst, const __m128i& _src)
	{
		_dst = _mm_cvtepu32_epi64(_src);
	}

	template<vmath_concept::Int_Type Int_Src_Tp, vmath_concept::Int_Type Int_Dst_Tp>
	constexpr auto castInt_Impl = cast_impl<__m128i, __m128i, Int_Src_Tp, Int_Dst_Tp>;

	/*
		Basic Addition Support For Floating Point Types	
	*/
	VMATH_INLINE __m128d add(__m128d x, __m128d y)
	{
		return _mm_add_pd(x, y);
	}

	VMATH_INLINE __m128 add(__m128 x, __m128 y)
	{
		return _mm_add_ps(x, y);
	}

	/*
		Basic Subtraction Support For Floating Point Types	
	*/

	VMATH_INLINE __m128d sub(__m128d x, __m128d y)
	{
		return _mm_sub_pd(x, y);
	}

	VMATH_INLINE __m128 sub(__m128 x, __m128 y)
	{
		return _mm_sub_ps(x, y);
	}

	/*
		Basic Multiplication Support For Floating Point Types	
	*/

	VMATH_INLINE __m128d mul(__m128d x, __m128d y)
	{
		return _mm_mul_pd(x, y);
	}

	VMATH_INLINE __m128 mul(__m128 x, __m128 y)
	{
		return _mm_mul_ps(x, y);
	}

	/*
		Basic Division Support For Floating Point Types	
	*/

	VMATH_INLINE __m128d div(__m128d x, __m128d y)
	{
		return _mm_div_pd(x, y);
	}

	VMATH_INLINE __m128 div(__m128 x, __m128 y)
	{
		return _mm_div_ps(x, y);
	}

	/*
		 Addition Support For Various Integral Types
	*/

	template<int Int_Size>
	VMATH_INLINE __m128i add(__m128i x, __m128i y);

	template<>
	VMATH_INLINE __m128i add<8>(__m128i x, __m128i y)
	{
		return _mm_add_epi8(x, y);
	}

	template<>
	VMATH_INLINE __m128i add<16>(__m128i x, __m128i y)
	{
		return _mm_add_epi16(x, y);
	}

	template<>
	VMATH_INLINE __m128i add<32>(__m128i x, __m128i y)
	{
		return _mm_add_epi32(x, y);
	}

	template<>
	VMATH_INLINE __m128i add<64>(__m128i x, __m128i y)
	{
		return _mm_add_epi64(x, y);
	}

	template<int Int_Size>
	VMATH_INLINE __m128i sub(__m128i x, __m128i y);

	template<>
	VMATH_INLINE __m128i sub<8>(__m128i x, __m128i y)
	{
		return _mm_sub_epi8(x, y);
	}

	template<>
	VMATH_INLINE __m128i sub<16>(__m128i x, __m128i y)
	{
		return _mm_sub_epi16(x, y);
	}

	template<>
	VMATH_INLINE __m128i sub<32>(__m128i x, __m128i y)
	{
		return _mm_sub_epi32(x, y);
	}

	template<>
	VMATH_INLINE __m128i sub<64>(__m128i x, __m128i y)
	{
		return _mm_sub_epi64(x, y);
	}


	/*
		 Multiplication Support For Various Integral Types
	*/

	template<int Int_Size>
	VMATH_INLINE __m128i mul(__m128i x, __m128i y);

	template<>
	VMATH_INLINE __m128i mul<8>(__m128i x, __m128i y)
	{
		x = _mm_cvtepi8_epi16(x);
		y = _mm_cvtepi8_epi16(y);
		x = _mm_mullo_epi16(x, y);
		return _mm_packs_epi16(x, x);
	}

	template<>
    VMATH_INLINE __m128i mul<16>(__m128i x, __m128i y)
	{
		return _mm_mullo_epi16(x, y);
	}

	template<>
    VMATH_INLINE __m128i mul<32>(__m128i x, __m128i y)
	{
		return _mm_mullo_epi32(x, y);
	}
};
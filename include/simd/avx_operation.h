#pragma once
#include "../common/common.h"
#include "../core/template_concept.h"
#include <immintrin.h>
#include <emmintrin.h>
#include <xmmintrin.h>

namespace vmath_concept
{
	template<typename T>
	struct is_avx_data 
	{
		static constexpr bool value =
			std::is_same_v<T, __m256> ||
			std::is_same_v<T, __m256d> ||
			std::is_same_v<T, __m256i>;
	};

	template<typename T>
	constexpr bool is_avx_data_v = is_avx_data<T>::value;

	template<typename T>
	concept Avx_Type = is_avx_data_v<T>;

	template<typename T>
	concept Avx_Int_Type = std::is_same_v<T, __m256i>;

	template<typename T>
	concept Avx_Float_Type = std::is_same_v<T, __m256> || std::is_same_v<T, __m256d>;
	
	template<typename T, typename U>
	struct is_avx_compatible
	{
		static constexpr bool value = false;
	};

	template<>
	struct is_avx_compatible<float, __m256>
	{
		static constexpr bool value = true;
	};

	template<>
	struct is_avx_compatible<double, __m256d>
	{
		static constexpr bool value = true;
	};

	template<Int_Type T>
	struct is_avx_compatible<T, __m256i>
	{
		static constexpr bool value = true;
	};
	
	template<typename T, typename U>
	constexpr bool is_avx_compatible_v = is_avx_compatible<T, U>::value;

	template<typename T>
	struct convert_to_avx;

	template<>
	struct convert_to_avx<double>
	{
		typedef __m256d type;
	};

	template<>
	struct convert_to_avx<float>
	{
		typedef __m256 type;
	};

	template<Int_Type T>
	struct convert_to_avx<T>
	{
		typedef __m256i type;
	};
	
	template<typename T>
	using convert_to_avx_t = typename convert_to_avx<T>::type;

	template<typename T>
	constexpr size_t avx_max_num_scalar_v = 256 / (sizeof(T) * 8);
}

namespace vmath_avx_templates{
	template<vconcept::Avx_Type T, vconcept::Avx_Type U,
		vconcept::Int_Type Int_Src_Tp = vconcept::EmptyType, vconcept::Int_Type Int_Dst_Tp = vconcept::EmptyType>
	VMATH_INLINE void cast_impl(T& _Dst, const U& _Src);
}

namespace vmath_avx
{
	using namespace vmath_avx_templates;
	/*
		Define type traits / concept helpers	
	*/
	namespace vconcept = vmath_concept;

	/*
		Loading function for AVX-1	
	*/
	template<bool IsAlign>
	VMATH_INLINE __m256 load(const float* _Src)
	{
		if constexpr (IsAlign)
		{
			return _mm256_load_ps(_Src);
		}
		else {
			return _mm256_loadu_ps(_Src);
		}
	}

	template<bool IsAlign>
	VMATH_INLINE __m256d load(const double* _Src)
	{
		if constexpr (IsAlign)
		{
			return _mm256_load_pd(_Src);
		}
		else {
			return _mm256_loadu_pd(_Src);
		}
	}

	template<bool IsAlign, vconcept::Int_Type T>
	VMATH_INLINE __m256i load(const T* _Src)
	{
		if constexpr (IsAlign)
		{
			return _mm256_load_si256(reinterpret_cast<const __m256i*>(_Src));
		}
		else
		{
			return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(_Src));
		}

	}

	VMATH_INLINE __m256 load(float _Scalar)
	{
		return _mm256_set1_ps(_Scalar);
	}

	VMATH_INLINE __m256 load(double _Scalar)
	{
		return _mm256_set1_pd(_Scalar);
	}

	VMATH_INLINE __m256 load(int8_t _Scalar)
	{
		return _mm256_set1_epi8(_Scalar);
	}

	VMATH_INLINE __m256 load(int16_t _Scalar)
	{
		return _mm256_set1_epi16(_Scalar);
	}

	VMATH_INLINE __m256 load(int32_t _Scalar)
	{
		return _mm256_set1_epi32(_Scalar);
	}

	VMATH_INLINE __m256 load(int64_t _Scalar)
	{
		return _mm256_set1_epi64x(_Scalar);
	}

	/*
		Mask Loading function for AVX-1	
	*/

	VMATH_INLINE __m256 maskLoad(const float* _Src, int _mask)
	{
		__m256i mask = _mm256_set_epi32(
			(_mask >> 7), (_mask >> 6), (_mask >> 5), (_mask >> 4),
			(_mask >> 3), (_mask >> 2), (_mask >> 1), _mask);
		return _mm256_maskload_ps(_Src, mask);
	}
	
	//Modified by Sam Zheng
	//(_mask >> n) & 1 replaced by (_mask >> n)
	VMATH_INLINE __m256d maskLoad(const double* _Src, int _mask)
	{
		__m256i mask = _mm256_set_epi64x((_mask >> 3), (_mask >> 2), (_mask >> 1), _mask);
		return _mm256_maskload_pd(_Src, mask);
	}

	VMATH_INLINE __m256i maskLoad(const int32_t* _Src, int _mask)
	{
		__m256i mask = _mm256_set_epi32(
			(_mask >> 7), (_mask >> 6), (_mask >> 5), (_mask >> 4),
			(_mask >> 3), (_mask >> 2), (_mask >> 1), _mask);
		return _mm256_maskload_epi32(_Src, mask);
	}

	VMATH_INLINE __m256i maskLoad(const int64_t* _Src, int _mask)
	{
		__m256i mask = _mm256_set_epi64x((_mask >> 3), (_mask >> 2), (_mask >> 1), _mask);
		return _mm256_maskload_epi64(_Src, mask);
	}

	/*
		Store function for AVX-1	
	*/

	template<bool IsAlign>
	VMATH_INLINE void store(float* _Dst, __m256& _Src);

	template<>
	VMATH_INLINE void store<false>(float* _Dst, __m256& _Src)
	{
		_mm256_storeu_ps(_Dst, _Src);
	}

	template<>
	VMATH_INLINE void store<true>(float* _Dst, __m256& _Src)
	{
		_mm256_store_ps(_Dst, _Src);
	}

	template<bool IsAlign>
	VMATH_INLINE void store(double* _Dst, __m256d& _Src);

	template<>
	VMATH_INLINE void store<false>(double* _Dst, __m256d& _Src)
	{
		_mm256_storeu_pd(_Dst, _Src);
	}

	template<>
	VMATH_INLINE void store<true>(double* _Dst, __m256d& _Src)
	{
		_mm256_store_pd(_Dst, _Src);
	}

	template<bool IsAlign, typename T>
	struct __IntegerStoreHelper
	{

	};

	template<typename U>
	struct __IntegerStoreHelper<false, U>
	{
		VMATH_INLINE static void store(U* _Dst, __m256i& _Src)
		{
			_mm256_storeu_si256(reinterpret_cast<__m256i*>(_Dst), _Src);
		}
	};

	template<typename U>
	struct __IntegerStoreHelper<true, U>
	{
		VMATH_INLINE static void store(U* _Dst, __m256i& _Src)
		{
			_mm256_store_si256(static_cast<__m256i*>(_Dst), _Src);
		}
	};


	template<bool IsAlign, typename T>
	VMATH_INLINE void store(T* _Dst, __m256i& _Src)
	{
		__IntegerStoreHelper<IsAlign, T>::store(_Dst, _Src);
	}


	/*
		Conversion function for AVX-1	
	*/

	template<>
	VMATH_INLINE void cast_impl(__m256d& _dst, const __m128i& _src)
	{
		_dst = _mm256_cvtepi32_pd(_src);
	}

	template<>
	VMATH_INLINE void cast_impl(__m256d& _dst, const __m128& _src)
	{
		_dst = _mm256_cvtps_pd(_src);
	}

	template<>
	VMATH_INLINE void cast_impl(__m256& _dst, const __m256i& _src)
	{
		_dst = _mm256_cvtepi32_ps(_src);
	}

	template<>
	VMATH_INLINE void cast_impl(__m128& _dst, const __m256d& _src)
	{
		_dst = _mm256_cvtpd_ps(_src);
	}

	template<>
	VMATH_INLINE void cast_impl(__m256i& _dst, const __m256& _src)
	{
		_dst = _mm256_cvtps_epi32(_src);
	}

	template<>
	VMATH_INLINE void cast_impl(__m128i& _dst, const __m256d& _src)
	{
		_dst = _mm256_cvtpd_epi32(_src);
	}

	/*
		Truncation function for AVX-1	
	*/

	VMATH_INLINE void trunc(__m128i& _dst, const __m256d& _src)
	{
		_dst = _mm256_cvttpd_epi32(_src);
	}

	VMATH_INLINE void trunc(__m256i& _dst, const __m256& _src)
	{
		_dst =  (_src);
	}


	/*
		Add function for AVX-1	
	*/
	
	VMATH_INLINE __m256d add(__m256d x, __m256d y)
	{
		return _mm256_add_pd(x, y);
	}

	VMATH_INLINE __m256 add(__m256 x, __m256 y)
	{
		return _mm256_add_ps(x, y);
	}

	/*
		Subtraction function for AVX-1	
	*/
	VMATH_INLINE __m256d sub(__m256d x, __m256d y)
	{
		return _mm256_sub_pd(x, y);
	}

	VMATH_INLINE __m256 sub(__m256 x, __m256 y)
	{
		return _mm256_sub_ps(x, y);
	}


	/*
		Multiplication function for AVX-1	
	*/
	VMATH_INLINE __m256d mul(__m256d x, __m256d y)
	{
		return _mm256_mul_pd(x, y);
	}

	VMATH_INLINE __m256 mul(__m256 x, __m256 y)
	{
		return _mm256_mul_ps(x, y);
	}

	/*
		Division function for AVX-1	
	*/
	VMATH_INLINE __m256d div(__m256d x, __m256d y)
	{
		return _mm256_div_pd(x, y);
	}

	VMATH_INLINE __m256 div(__m256 x, __m256 y)
	{
		return _mm256_div_ps(x, y);
	}
};

namespace vmath_avx2
{
	using namespace vmath_avx;
	using namespace vmath_avx_templates;

	namespace vconcept = vmath_concept;

	/*
		Conversion function for AVX-2	
	*/

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, int8_t, int16_t>(__m256i& _dst, const __m128i& _src)
	{
		_dst = _mm256_cvtepi8_epi16(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, int8_t, int32_t>(__m256i& _dst, const __m128i& _src)
	{
		_dst = _mm256_cvtepi8_epi32(_src);
	}


	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, int8_t, int64_t>(__m256i& _dst, const __m128i& _src)
	{
		_dst = _mm256_cvtepi8_epi64(_src);
	}


	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, int16_t, int32_t>(__m256i& _dst, const __m128i& _src)
	{
		_dst = _mm256_cvtepi16_epi32(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, int16_t, int64_t>(__m256i& _dst, const __m128i& _src)
	{
		_dst = _mm256_cvtepi16_epi64(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, int32_t, int64_t>(__m256i& _dst, const __m128i& _src)
	{
		_dst = _mm256_cvtepi32_epi64(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, uint8_t, int16_t>(__m256i& _dst, const __m128i& _src)
	{
		_dst = _mm256_cvtepu8_epi16(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, uint8_t, int32_t>(__m256i& _dst, const __m128i& _src)
	{
		_dst = _mm256_cvtepu8_epi32(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, uint8_t, int64_t>(__m256i& _dst, const __m128i& _src)
	{
		_dst = _mm256_cvtepu8_epi64(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, uint16_t, int32_t>(__m256i& _dst, const __m128i& _src)
	{
		_dst = _mm256_cvtepu16_epi32(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, uint16_t, int64_t>(__m256i& _dst, const __m128i& _src)
	{
		_dst = _mm256_cvtepu16_epi64(_src);
	}

	template<>
	VMATH_INLINE void cast_impl<__m128i, __m128i, uint32_t, int64_t>(__m256i& _dst, const __m128i& _src)
	{
		_dst = _mm256_cvtepu32_epi64(_src);
	}

	template<vconcept::Int_Type Int_Src_Tp, vconcept::Int_Type Int_Dst_Tp>
	constexpr auto castInt_impl = cast_impl<__m128i, __m128i, Int_Src_Tp, Int_Dst_Tp>;

	/*
		Addition function for AVX-2	
	*/
	template<int Int_Size>
	VMATH_INLINE __m256i add(__m256i x, __m256i y);

	template<>
	VMATH_INLINE __m256i add<8>(__m256i x, __m256i y)
	{
		return _mm256_add_epi8(x, y);
	}

	template<>
	VMATH_INLINE __m256i add<16>(__m256i x, __m256i y)
	{
		return _mm256_add_epi16(x, y);
	}

	template<>
	VMATH_INLINE __m256i add<32>(__m256i x, __m256i y)
	{
		return _mm256_add_epi32(x, y);
	}

	template<>
	VMATH_INLINE __m256i add<64>(__m256i x, __m256i y)
	{
		return _mm256_add_epi64(x, y);
	}

	/*
		Subtraction function for AVX-2	
	*/
	template<int Int_Size>
	VMATH_INLINE __m256i sub(__m256i x, __m256i y);

	template<>
	VMATH_INLINE __m256i sub<8>(__m256i x, __m256i y)
	{
		return _mm256_sub_epi8(x, y);
	}

	template<>
	VMATH_INLINE __m256i sub<16>(__m256i x, __m256i y)
	{
		return _mm256_sub_epi16(x, y);
	}

	template<>
	VMATH_INLINE __m256i sub<32>(__m256i x, __m256i y)
	{
		return _mm256_sub_epi32(x, y);
	}

	template<>
	VMATH_INLINE __m256i sub<64>(__m256i x, __m256i y)
	{
		return _mm256_sub_epi64(x, y);
	}

	/*
		Multiplication function for AVX-2	
	*/
	template<int Int_Size, bool Unsigned = false>
	VMATH_INLINE __m256i mul(__m256i x, __m256i y);

	template<>
    VMATH_INLINE __m256i mul<32, false>(__m256i x, __m256i y)
	{
		return _mm256_mul_epi32(x, y);
	}

	template<>
	VMATH_INLINE __m256i mul<32, true>(__m256i x, __m256i y)
	{
		return _mm256_mul_epu32(x, y);
	}
};

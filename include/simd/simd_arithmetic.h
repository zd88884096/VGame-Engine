#pragma once
#include "../core/template_concept.h"
#include "simd_operation.h"


namespace vmath_simd_arithmetic
{
	// SSE Implementations
	template<bool Return_Align = false, bool Align1 = false,
			bool Align2 = false, vmath_concept::Fp_Type T>
		void vecAdd_Sse128Impl(T* _Dst, const T* _Src1, const T* _Src2)
	{
		auto x = vmath_sse::load<Align1>(_Src1);
		auto y = vmath_sse::load<Align2>(_Src2);
		x = vmath_sse::add(x, y);
		vmath_sse::store<Return_Align>(_Dst, x);
	}

	template<bool Return_Align = false, bool Align1 = false,
			bool Align2 = false, vmath_concept::Fp_Type T>
		void vecSub_Sse128Impl(T* _Dst, const T* _Src1, const T* _Src2)
	{
		auto x = vmath_sse::load<Align1>(_Src1);
		auto y = vmath_sse::load<Align2>(_Src2);
		x = vmath_sse::sub(x, y);
		vmath_sse::store<Return_Align>(_Dst, x);
	}

	template<bool Return_Align = false, bool Align1 = false,
			bool Align2 = false, vmath_concept::Fp_Type T>
		void vecCwMul_Sse128Impl(T* _Dst, const T* _Src1, const T* _Src2)
	{
		auto x = vmath_sse::load<Align1>(_Src1);
		auto y = vmath_sse::load<Align2>(_Src2);
		x = vmath_sse::mul(x, y);
		vmath_sse::store<Return_Align>(_Dst, x);
	}

	template<bool Return_Align = false, bool Align1 = false,
			bool Align2 = false, vmath_concept::Fp_Type T>
		void vecMul_Sse128Impl(T* _Dst, const T* _Src1, T _Scalar)
	{
		auto x = vmath_sse::load<Align1>(_Src1);
		auto y = _mm_set1_pd(_Scalar);
		x = vmath_sse::mul(x, y);
		vmath_sse::store<Return_Align>(_Dst, x);
	}

	template<bool Return_Align = false, bool Align1 = false,
			bool Align2 = false, vmath_concept::Fp_Type T>
		void vecCwDiv_Sse128Impl(T* _Dst, const T* _Src1, const T* _Src2)
	{
		auto x = vmath_sse::load<Align1>(_Src1);
		auto y = vmath_sse::load<Align2>(_Src2);
		x = vmath_sse::div(x, y);
		vmath_sse::store<Return_Align>(_Dst, x);
	}

	template<bool Return_Align = false, bool Align1 = false,
			bool Align2 = false, vmath_concept::Fp_Type T>
		void vecDiv_Sse128Impl(T* _Dst, const T* _Src1, T _Scalar)
	{
		auto x = vmath_sse::load<Align1>(_Src1);
		auto y = _mm_set1_pd(static_cast<T>(1.0) / _Scalar);
		x = vmath_sse::mul(x, y);
		vmath_sse::store<Return_Align>(_Dst, x);
	}

	// Avx Implementations
	template<bool Return_Align = false, bool Align1 = false, 
			bool Align2 = false, vmath_concept::Fp_Type T>
		void vecAdd_Avx256Impl(T* _Dst, const T* _Src1, const T* _Src2)
	{
		auto x = vmath_avx::load<Align1>(_Src1);
		auto y = vmath_avx::load<Align2>(_Src2);
		x = vmath_avx::add(x, y);
		vmath_avx::store<Return_Align>(_Dst, x);
	}

	template<bool Return_Align = false, bool Align1 = false, 
			bool Align2 = false, vmath_concept::Int_Type T>
	void vecAdd_Avx256Impl(T* _Dst, const T* _Src1, const T* _Src2)
	{
		auto x = vmath_avx::load<Align1>(_Src1);
		auto y = vmath_avx::load<Align2>(_Src2);
		x = vmath_avx2::add<sizeof(T)>(x, y);
		vmath_avx::store<Return_Align>(_Dst, x);
	}

	template<bool Return_Align = false, bool Align1 = false, 
			bool Align2 = false, vmath_concept::Fp_Type T>
	void vecSub_Avx256Impl(T* _Dst, const T* _Src1, const T* _Src2)
	{
		auto x = vmath_avx::load<Align1>(_Src1);
		auto y = vmath_avx::load<Align2>(_Src2);
		x = vmath_avx2::sub(x, y);
		vmath_avx::store<Return_Align>(_Dst, x);
	}

	template<bool Return_Align = false, bool Align1 = false, 
			bool Align2 = false, vmath_concept::Int_Type T>
	void vecSub_Avx256Impl(T* _Dst, const T* _Src1, const T* _Src2)
	{
		auto x = vmath_avx::load<Align1>(_Src1);
		auto y = vmath_avx::load<Align2>(_Src2);
		x = vmath_avx::sub<sizeof(T)>(x, y);
		vmath_avx::store<Return_Align>(_Dst, x);
	}

	template<bool Return_Align = false, bool Align1 = false, 
			bool Align2 = false, vmath_concept::Fp_Type T>
	void vecCwMul_Avx256Impl(T* _Dst, const T* _Src1, const T* _Src2)
	{
		auto x = vmath_avx::load<Align1>(_Src1);
		auto y = vmath_avx::load<Align2>(_Src2);
		x = vmath_avx::mul(x, y);
		vmath_avx::store<Return_Align>(_Dst, x);
	}




	template<bool Return_Align = false, bool Align1 = false, 
			bool Align2 = false, vmath_concept::Fp_Type T>
	void vecDiv_Avx256Impl(T* _Dst, const T* _Src1, T _Scalar)
	{
		auto x = vmath_avx::load<Align1>(_Src1);
		auto y = _mm256_set1_pd((static_cast<T>(1.0) / _Scalar));
		x = vmath_avx::mul(x, y);
		vmath_avx::store<Return_Align>(_Dst, x);
	}

	template<bool Return_Align = false, bool Align1 = false, 
			bool Align2 = false, vmath_concept::Fp_Type T>
	void vecCwDiv_Avx256Impl(T* _Dst, const T* _Src1, const T* _Src2)
	{
		auto x = vmath_avx::load<Align1>(_Src1);
		auto y = vmath_avx::load<Align2>(_Src2);
		x = vmath_avx::div(x, y);
		vmath_avx::store<Return_Align>(_Dst, x);
	}

};

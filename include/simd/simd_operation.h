#pragma once
#include "../common/common.h"
#include "../core/template_concept.h"
#include "avx_operation.h"
#include "sse_operation.h"


namespace vmath_concept
{
	template<typename T>
	concept Simd_Type = Sse_Type<T> || Avx_Type<T>;

	template<typename T, typename U>
	concept is_simd_compatible_v = is_avx_compatible_v<T, U> || is_sse_compatible_v<T, U>;

}

namespace vmath_simd
{
#ifdef VMATH_SSE
	inline namespace vmath_sse { using namespace ::vmath_sse; }
#endif
#ifdef VMATH_AVX
	inline namespace vmath_avx { using namespace ::vmath_avx; }
	inline namespace vmath_avx2 { using namespace ::vmath_avx2; }
#endif
};

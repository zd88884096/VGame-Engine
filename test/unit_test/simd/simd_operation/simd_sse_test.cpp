#include "simd/sse_operation.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "timer.h"
#include <iostream>
#include <type_traits>
#include <functional>
#include <algorithm>

using namespace ::testing;
template<typename Array_Tp1, typename Array_Tp2, size_t Array_Len, typename Epsilon_Tp = double>
void EpsilonArrayEqualCheck(const Array_Tp1(&_Array1)[Array_Len], const Array_Tp2(&_Array2)[Array_Len], Epsilon_Tp _Epsilon = 0.0000001)
{
	EXPECT_THAT(_Array1, Pointwise(FloatNear(_Epsilon), _Array2));
}



using namespace vmath_sse;



int main(int argc, char ** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(SseTest, LoadStoreTest)
{
	auto storeCheck = []<typename T, size_t Array_Len = 128 / (4 * sizeof(T))> (const T (&_SrcArray)[Array_Len], bool _LoadAligned) -> void
	{
		
		auto loadData = (_LoadAligned) ? load<true>(_SrcArray) : load<false>(_SrcArray);

		T unalignedStoreArray[Array_Len] = {};
		alignas(16) T alignedStoreArray16[Array_Len] = {};
		
		if constexpr (std::is_integral_v<T>)
		{
			store<false, T>(unalignedStoreArray, loadData);
			EXPECT_THAT(_SrcArray, ElementsAreArray(unalignedStoreArray));
			store<true, T>(alignedStoreArray16, loadData);
			EXPECT_THAT(_SrcArray, ElementsAreArray(alignedStoreArray16));
		}
		else
		{
			store<false>(unalignedStoreArray, loadData);
			EpsilonArrayEqualCheck(_SrcArray, unalignedStoreArray);
			
			store<true>(alignedStoreArray16, loadData);
			EpsilonArrayEqualCheck(_SrcArray, alignedStoreArray16);
		}
	};

	// Float Load / Store Test
	{
		// Unaligned Array Load
		{
			constexpr float testArray[] = { 1.0f, 0.0f, -4.9f, 1234789.9f};
			auto loadData = load<false>(testArray);
			
			storeCheck(testArray, false);
		}
		

		// Aligned Array Load
		{
			alignas(16) constexpr float testArray[] = { std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest(), -4.9f, 1234589.9f};
			auto loadData = load<true>(testArray);

			storeCheck(testArray, true);
		}
	}


	// Double Load / Store Test
	{
		// Unaligned Array Load
		{
			constexpr double testArray[] = { -5234114.47582034, 947501.498510101 };
			auto loadData = load<false>(testArray);

			storeCheck(testArray, false);
		}

		// Aligned Array Load
		{
			alignas(16) constexpr double testArray[] = { std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max() };

			storeCheck(testArray, true);
		}
	}


	// Integral Type Load / Store Test
	{
		// Int 8 Load
		{
			// Unaligned Array Load
			{
				constexpr int8_t testArray[] = { 127, -121, 3, 4, 5, -1, 3, 0, 9, 12, 4, -54, 86, 94, -100, 99};
				
				storeCheck(testArray, false);
			}

			// Aligned Array Load
			{
				alignas(8) constexpr int8_t testArray[] = { 127, -128, 3, 4, 5, -1, 3, 0, 9, 12, 4, -54, 86, 94, -100, 99};
				
				storeCheck(testArray, true);
			}
		}

		// Int 16 Load
		{
			// Unaligned Array Load
			{
				constexpr int16_t testArray[] = { 127, -121, 3, 4, 5, -1, 255, -256};
				
				storeCheck(testArray, false);
			}

			// Aligned Array Load
			{
				alignas(16) constexpr int16_t testArray[] = { 127, -121, 3, 4, 5, -1, 255, -256};
				
				storeCheck(testArray, true);
			}
		}

		// Int 32 Load
		{
			// Unaligned Array Load
			{
				constexpr int32_t testArray[] = { 127, 0, std::numeric_limits<int>::max(), std::numeric_limits<int>::lowest()};
				
				storeCheck(testArray, false);
			}

			// Aligned Array Load
			{
				alignas(32) constexpr int32_t testArray[] = { 127, 0, std::numeric_limits<int>::max(), std::numeric_limits<int>::lowest()};
				
				storeCheck(testArray, true);
			}
		}

		// Int 64 Load
		{
			// Unaligned Array Load
			{
				constexpr int64_t testArray[] = { std::numeric_limits<int64_t>::max(), 0};
				
				storeCheck(testArray, false);
			}

			// Aligned Array Load
			{
				alignas(64) constexpr int64_t testArray[] = { std::numeric_limits<int>::max(), std::numeric_limits<int64_t>::lowest()};
				
				storeCheck(testArray, true);
			}
		}
	}
}


TEST(SseTest, MaskLoadTest)
{

	// Float Mask Load
	{
		constexpr float testArray[] = { 1.0f, 0.0f, -4.9f, 12346790.0f};
		constexpr float correctArray[] = { 1.0f, 0.0f, 0.0f, 12346790.0f };
		int _mask = 9;
		float loadArray[4];
		auto loadData = maskLoad(testArray, _mask);
		store<false>(loadArray, loadData);
		EpsilonArrayEqualCheck(loadArray, correctArray);
	}

	// Double Mask Load
	{
		constexpr double testArray[] = { -5234114.47582034, 947501.498510101 };
		constexpr double correctArray[] = { -5234114.47582034, 0 };
		int _mask = 2;
		double loadArray[2];
		auto loadData = maskLoad(testArray, _mask);
		store<false>(loadArray, loadData);
		EpsilonArrayEqualCheck(loadArray, correctArray);
	}

	// Int 32 Mask Load
	{
		constexpr int32_t testArray[] = { 357, 566, 820, 666 };
		constexpr int32_t correctArray[] = { 0, 566, 820, 666 };
		int _mask = 7;
		int32_t loadArray[4];
		auto loadData = maskLoad(testArray, _mask);
		store<false>(loadArray, loadData);
		EpsilonArrayEqualCheck(loadArray, correctArray);
	}

	// Int 64 Mask Load
	{
		constexpr int64_t testArray[] = { -124524, std::numeric_limits<int64_t>::max() };
		constexpr int64_t correctArray[] = { 0, std::numeric_limits<int64_t>::max() };
		int _mask = 1;
		int64_t loadArray[2];
		auto loadData = maskLoad(testArray, _mask);
		store<false>(loadArray, loadData);
		EpsilonArrayEqualCheck(loadArray, correctArray);
	}
}

TEST(SseTest, CastTest)
{
	auto testCast = [](const auto& testArray, const auto& correctArray, auto& loadArray, const std::string& _Error_Msg = "", double _Epsilon = 0.0001)
	{
		auto loadData = load<false>(testArray);
		__m128i storeData;
		
		typedef std::decay_t<decltype(testArray[0])> Src_Element_Tp;
		typedef std::decay_t<decltype(correctArray[0])> Dst_Element_Tp;
		constexpr bool IntToIntCast = vmath_concept::Int_Type<Src_Element_Tp> && vmath_concept::Int_Type<Dst_Element_Tp>;

		if constexpr (IntToIntCast) cast_impl<__m128i, __m128i, Src_Element_Tp, Dst_Element_Tp>(storeData, loadData);
		else cast_impl(storeData, loadData);

		store<false>(loadArray, storeData);
		if constexpr (IntToIntCast)
		{
			EXPECT_THAT(loadArray, ElementsAreArray(correctArray));
		}
		else
		{
			EXPECT_THAT(loadArray, Pointwise(DoubleNear(_Epsilon), correctArray)) << _Error_Msg.c_str();
		}

	};
	// Float Cast Test
	{
		// To Int 32
		{
			constexpr float testArray[] = { 49.24, 13.27, 99.12, -40.34 };
			constexpr int32_t correctArray[] = { 49, 13, 99, -40 };

			auto loadData = load<false>(testArray);
			__m128i storeData;

			int32_t loadArray[4];
			cast_impl(storeData, loadData);
			store<false>(loadArray, storeData);
			EXPECT_THAT(loadArray, ElementsAreArray(correctArray));
		}

		// To Double
		{
			constexpr float testArray[] = { 13.04, 27.66, -93.24, -99.999 };
			constexpr double correctArray[] = { 13.04, 27.66 };

			auto loadData = load<false>(testArray);
			__m128d storeData;

			double loadArray[2];
			cast_impl(storeData, loadData);
			store<false>(loadArray, storeData);
			// Float to Double may induce some problem in precision
			EpsilonArrayEqualCheck(correctArray, loadArray, 0.001);
		}
	}

	// Double Cast Test
	{
		// To Int 32
		{
			constexpr double testArray[] = { 3.1415, -9.26535 };
			constexpr int32_t correctArray[] = { 3, -9 , 0, 0 };

			auto loadData = load<false>(testArray);
			__m128i storeData;

			int32_t loadArray[4];
			cast_impl(storeData, loadData);
			store<false>(loadArray, storeData);
			EXPECT_THAT(loadArray, ElementsAreArray(correctArray));
		}

		// To Float
		{

			constexpr double testArray[] = { 3.1415, -9.26535 };
			constexpr float correctArray[] = { 3.1415, -9.26535, 0, 0 };

			auto loadData = load<false>(testArray);
			__m128 storeData;

			float loadArray[4];
			cast_impl(storeData, loadData);
			store<false>(loadArray, storeData);
			EpsilonArrayEqualCheck(correctArray, loadArray);
		}
	}

	// Int Cast Test
	{
		// To Float
		{
			constexpr int32_t testArray[] = { 3, -2, 99, 100 }; 
			constexpr float correctArray[] = { 3, -2, 99, 100 };

			auto loadData = load<false>(testArray);
			__m128 storeData;

			float loadArray[4];
			cast_impl(storeData, loadData);
			store<false>(loadArray, storeData);
			EpsilonArrayEqualCheck(correctArray, loadArray);
		}

		// To Double
		{
			constexpr int32_t testArray[] = { std::numeric_limits<int32_t>::max(), -1, 3, 4 };
			constexpr double correctArray[] = { std::numeric_limits<int32_t>::max(), -1 };

			auto loadData = load<false>(testArray);
			__m128d storeData;

			double loadArray[2];
			cast_impl(storeData, loadData);
			store<false>(loadArray, storeData);
			EpsilonArrayEqualCheck(correctArray, loadArray);
		}

		// Int 8 To Int 16
		{
			constexpr int8_t testArray[] = {
				23, 19, -49, 127,
				33, 10, 25, 10, 
				13, 14, 0, -1, 
				57, 63, -79, 90
			};
			constexpr int16_t correctArray[] =
			{
				23, 19, -49, 127,
				33, 10, 25, 10
			};
			auto loadData = load<false>(testArray);
			__m128i storeData;

			int16_t loadArray[8];
			castInt_Impl<int8_t, int16_t>(storeData, loadData);
			store<false>(loadArray, storeData);
			EXPECT_THAT(loadArray, ElementsAreArray(correctArray)) << "Error Int8 to Int16 Cast!";
		}

		// Int 8 To Int 32
		{
			constexpr int8_t testArray[] = {
				23, 19, -49, 127,
				33, 10, 25, 10, 
				13, 14, 0, -1, 
				57, 63, -79, 90
			};
			constexpr int32_t correctArray[] =
			{
				23, 19, -49, 127
			};

			int32_t loadArray[4];

			testCast(testArray, correctArray, loadArray, "Error Int8 to Int16 Cast!");
		}

	}

}





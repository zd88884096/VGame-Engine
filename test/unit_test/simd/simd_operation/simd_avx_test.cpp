#include "../../../../include/simd/sse_operation.h"
#include "../../../../include/simd/avx_operation.h"
#include "/gtest/gtest.h"
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



using namespace vmath_avx;
using namespace vmath_avx2;


int main(int argc, char ** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(AvxTest, LoadStoreTest)
{
	auto storeCheck = []<typename T, size_t Array_Len = 256 / (8 * sizeof(T))> (const T (&_SrcArray)[Array_Len], bool _LoadAligned) -> void
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
			constexpr float testArray[] = { 1.0f, 0.0f, -4.9f, 1234789.9f, 3.6f, -3.228382f, -2.3223f, -134134112f};
			auto loadData = load<false>(testArray);
			storeCheck(testArray, false);
		}
		

		// Aligned Array Load
		{
			alignas(16) constexpr float testArray[] = { std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest(), -4.9f, 1234589.9f, 0.0f, -3.228382f, -2.3223f, -134134112f};
			auto loadData = load<true>(testArray);
			storeCheck(testArray, true);
		}
	}


	// Double Load / Store Test
	{
		// Unaligned Array Load
		{
			constexpr double testArray[] = { -5234114.47582034, 947501.498510101, std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max()};
			auto loadData = load<false>(testArray);

			storeCheck(testArray, false);
		}

		// Aligned Array Load
		{
			alignas(16) constexpr double testArray[] = { std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(), -1231394.1114123, 199139414.013};
			auto loadData = load<true>(testArray);
			storeCheck(testArray, true);
		}
	}


	// Integral Type Load / Store Test
	{
		// Int 8 Load
		{
			// Unaligned Array Load
			{
				constexpr int8_t testArray[] = { 127, -121, 3, 4, 5, -1, 3, 0, 9, 12, 4, -54, 86, 94, -100, 99, 46, 33, -46, -73, 104, 19, -23, 89, 93, 110, 102, 42, 36, 115, -118, 80 };
				
				storeCheck(testArray, false);
			}

			// Aligned Array Load
			{
				alignas(8) constexpr int8_t testArray[] = { 127, -128, 3, 4, 5, -1, 3, 0, 9, 12, 4, -54, 86, 94, -100, 99, 5, -11, 107, -29, -82, -38, 88, -51, 67, -29, 98, -8, 58, -59, 104, -122};
				
				storeCheck(testArray, true);
			}
		}

		// Int 16 Load
		{
			// Unaligned Array Load
			{
				constexpr int16_t testArray[] = { 127, -121, 3, 4, 5, -1, 255, -256, -25119, -1874, -25336, 25425, -25019, 9123, 25955, 2285};
				
				storeCheck(testArray, false);
			}

			// Aligned Array Load
			{
				alignas(16) constexpr int16_t testArray[] = { 127, -121, 3, 4, 5, -1, 255, -256, -24543, -22497, -13994, -9980, 23847, 13068, 26908, -8241};
				
				storeCheck(testArray, true);
			}
		}

		// Int 32 Load
		{
			// Unaligned Array Load
			{
				constexpr int32_t testArray[] = { 127, 0, std::numeric_limits<int>::max(), std::numeric_limits<int>::lowest(), 554921320, -757079210, -443071427, -845002044};
				
				storeCheck(testArray, false);
			}

			// Aligned Array Load
			{
				alignas(32) constexpr int32_t testArray[] = { 127, 0, std::numeric_limits<int>::max(), std::numeric_limits<int>::lowest(), -1098653733, -565939195, -1034807643, 2047836064};
				
				storeCheck(testArray, true);
			}
		}

		// Int 64 Load
		{
			// Unaligned Array Load
			{
				constexpr int64_t testArray[] = { std::numeric_limits<int64_t>::max(), 0, 1246244236984490718, -1759707991122475938};
				
				storeCheck(testArray, false);
			}

			// Aligned Array Load
			{
				alignas(64) constexpr int64_t testArray[] = { std::numeric_limits<int>::max(), std::numeric_limits<int64_t>::lowest(), -1943286453332066975, -1344393161029074663};
				
				storeCheck(testArray, true);
			}
		}
	}
}


TEST(AvxTest, MaskLoadTest)
{

	// Float Mask Load
	{
		constexpr float testArray[] = { 1.0f, 0.0f, -4.9f, 1234789.9f, 3.6f, -3.228382f, -2.3223f, -134134112f};
		constexpr float correctArray[] = { 1.0f, 0.0f, 0.0f, 1234789.9f, 3.6f, 0.0f, -2.3223f, 0.0f };
		int _mask = 218;
		float loadArray[8];
		auto loadData = maskLoad(testArray, _mask);
		store<false>(loadArray, loadData);
		EpsilonArrayEqualCheck(loadArray, correctArray);
	}

	// Double Mask Load
	{
		constexpr double testArray[] = { -5234114.47582034, 947501.498510101, std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max() };
		constexpr double correctArray[] = { -5234114.47582034, 0, 0, std::numeric_limits<double>::max()};
		int _mask = 9;
		double loadArray[4];
		auto loadData = maskLoad(testArray, _mask);
		store<false>(loadArray, loadData);
		EpsilonArrayEqualCheck(loadArray, correctArray);
	}

	// Int 32 Mask Load
	{
		constexpr int32_t testArray[] = { 357, 566, 820, 666, 554921320, -757079210, -443071427, -845002044 };
		constexpr int32_t correctArray[] = { 0, 566, 820, 666, 0, 0, -443071427, -845002044 };
		int _mask = 115;
		int32_t loadArray[8];
		auto loadData = maskLoad(testArray, _mask);
		store<false>(loadArray, loadData);
		EpsilonArrayEqualCheck(loadArray, correctArray);
	}

	// Int 64 Mask Load
	{
		constexpr int64_t testArray[] = { -124524, std::numeric_limits<int64_t>::max(), 1246244236984490718, -1759707991122475938 };
		constexpr int64_t correctArray[] = { 0, std::numeric_limits<int64_t>::max(), 1246244236984490718, 0 };
		int _mask = 6;
		int64_t loadArray[4];
		auto loadData = maskLoad(testArray, _mask);
		store<false>(loadArray, loadData);
		EpsilonArrayEqualCheck(loadArray, correctArray);
	}
}

TEST(AvxTest, CastTest)
{
	auto testCast = [](const auto& testArray, const auto& correctArray, auto& loadArray, const std::string& _Error_Msg = "", double _Epsilon = 0.0001)
	{
		auto loadData = load<false>(testArray);
		__m256i storeData;
		
		typedef std::decay_t<decltype(testArray[0])> Src_Element_Tp;
		typedef std::decay_t<decltype(correctArray[0])> Dst_Element_Tp;
		constexpr bool  = vmath_concept::Int_Type<Src_Element_Tp> && vmath_concept::Int_Type<Dst_Element_Tp>;

		if constexpr (IntToIntCast) cast_impl<__m256i, __m256i, Src_Element_Tp, Dst_Element_Tp>(storeData, loadData);
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
			constexpr float testArray[] = { 49.24, 13.27, 99.12, -40.34, 14.234, 1881.2, 90.223, 11.35 };
			constexpr int32_t correctArray[] = { 49, 13, 99, -40, 14, 1881, 90, 11 };

			auto loadData = load<false>(testArray);
			__m256i storeData;

			int32_t loadArray[8];
			cast_impl(storeData, loadData);
			store<false>(loadArray, storeData);
			EXPECT_THAT(loadArray, ElementsAreArray(correctArray));
		}

		// To Double
		{
			constexpr float testArray[] = { 13.04, 27.66, -93.24, -99.999, 14.234, 1881.2, 90.223, 11.35 };
			constexpr double correctArray[] = { 13.04, 27.66, -93.24, -99.999 };

			auto loadData = load<false>(testArray);
			__m256d storeData;

			double loadArray[4];
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
			constexpr double testArray[] = { 3.1415, -9.26535, 1.2352, -13925.22234 };
			constexpr int32_t correctArray[] = { 3, -9 , 1, -13925, 0, 0, 0, 0 };

			auto loadData = load<false>(testArray);
			__m256i storeData;

			int32_t loadArray[8];
			cast_impl(storeData, loadData);
			store<false>(loadArray, storeData);
			EXPECT_THAT(loadArray, ElementsAreArray(correctArray));
		}

		// To Float
		{

			constexpr double testArray[] = { 3.1415, -9.26535, 1.2352, -13925.22234 };
			constexpr float correctArray[] = {3.1415, -9.26535, 1.2352, -13925.22234, 0, 0, 0, 0 };

			auto loadData = load<false>(testArray);
			__m256 storeData;

			float loadArray[8];
			cast_impl(storeData, loadData);
			store<false>(loadArray, storeData);
			EpsilonArrayEqualCheck(correctArray, loadArray);
		}
	}

	// Int Cast Test
	{
		// To Float
		{
			constexpr int32_t testArray[] = { 3, -2, 99, 100, 13, 22252, 35252325, 122 }; 
			constexpr float correctArray[] = { 3, -2, 99, 100, 13, 22252, 35252325, 122 };

			auto loadData = load<false>(testArray);
			__m256 storeData;

			float loadArray[8];
			cast_impl(storeData, loadData);
			store<false>(loadArray, storeData);
			EpsilonArrayEqualCheck(correctArray, loadArray);
		}

		// To Double
		{
			constexpr int32_t testArray[] = { std::numeric_limits<int32_t>::max(), -1, 3, 4, 134, 1414, 1151, -1243243 };
			constexpr double correctArray[] = { std::numeric_limits<int32_t>::max(), -1, 3, 4 };

			auto loadData = load<false>(testArray);
			__m256d storeData;

			double loadArray[4];
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
				57, 63, -79, 90,
				46, 33, -46, -73, 
				104, 19, -23, 89, 
				93, 110, 102, 42, 
				36, 115, -118, 80

			};
			constexpr int16_t correctArray[] =
			{
				23, 19, -49, 127,
				33, 10, 25, 10,
				13, 14, 0, -1, 
				57, 63, -79, 90
			};
			auto loadData = load<false>(testArray);
			__m256i storeData;

			int16_t loadArray[16];
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
				57, 63, -79, 90,
				46, 33, -46, -73, 
				104, 19, -23, 89, 
				93, 110, 102, 42, 
				36, 115, -118, 80

			};
			constexpr int32_t correctArray[] =
			{
				23, 19, -49, 127,
				33, 10, 25, 10
			};

			int32_t loadArray[8];

			testCast(testArray, correctArray, loadArray, "Error Int8 to Int32 Cast!");
		}

		// Int 8 To Int 64
		{
			constexpr int8_t testArray[] = {
				23, 19, -49, 127,
				33, 10, 25, 10, 
				13, 14, 0, -1, 
				57, 63, -79, 90,
				46, 33, -46, -73, 
				104, 19, -23, 89, 
				93, 110, 102, 42, 
				36, 115, -118, 80

			};
			constexpr int64_t correctArray[] =
			{
				23, 19, -49, 127
			};

			int64_t loadArray[4];

			testCast(testArray, correctArray, loadArray, "Error Int8 to Int64 Cast!");
		}

		// Int 16 To Int 32
		{
			constexpr int16_t testArray[] = {
				23, 19, -49, 127,
				33, 10, 25, 10, 
				13, 14, 0, -1, 
				57, 63, -79, 90

			};
			constexpr int32_t correctArray[] =
			{
				23, 19, -49, 127,
				33, 10, 25, 10
			};

			int32_t loadArray[8];

			testCast(testArray, correctArray, loadArray, "Error Int16 to Int32 Cast!");
		}

		// Int 16 To Int 64
		{
			constexpr int16_t testArray[] = {
				23, 19, -49, 127,
				33, 10, 25, 10, 
				13, 14, 0, -1, 
				57, 63, -79, 90

			};
			constexpr int64_t correctArray[] =
			{
				23, 19, -49, 127
			};

			int32_t loadArray[4];

			testCast(testArray, correctArray, loadArray, "Error Int16 to Int64 Cast!");
		}

		// Int 32 To Int 64
		{
			constexpr int32_t testArray[] = {
				23, 19, -49, 127,
				33, 10, 25, 10

			};
			constexpr int64_t correctArray[] =
			{
				23, 19, -49, 127
			};

			int32_t loadArray[4];

			testCast(testArray, correctArray, loadArray, "Error Int32 to Int64 Cast!");
		}
	}
}

TEST(AvxTest, TruncTest)
{
	auto testTrunc = [](const auto& testArray, const auto& correctArray, auto& loadArray, const std::string& _Error_Msg = "", double _Epsilon = 0.0001)
	{
		auto loadData = load<false>(testArray);
		
		
		typedef std::decay_t<decltype(testArray[0])> Src_Element_Tp;
		typedef std::decay_t<decltype(correctArray[0])> Dst_Element_Tp;
		constexpr bool FloatToIntTrunc = vmath_concept::Avx_Type<Src_Element_Tp> && vmath_concept::Avx_Type<Dst_Element_Tp>;

		if constexpr (FloatToIntTrunc)
		{
			__m256i storeData;
			trunc(storeData, loadData);
			store<false>(loadArray, storeData);
		}
		else 
		{
			__m128i storeData;
			trunc(storeData, loadData);
			vmath_sse::store<false>(loadArray, storeData);
		}

		EXPECT_THAT(loadArray, Pointwise(DoubleNear(_Epsilon), correctArray)) << _Error_Msg.c_str();
	};

	//double to int32 trunc
	{
		constexpr double testArray[] = {1.00,2.58, 22523.225, -1431.252};
		constexpr int32_t correctArray[] = {1,2,22523, -1431};

		int32_t loadArray[4];

		testTrunc(testArray, correctArray, loadArray, "Error Double to Int32 Trunc!");
	}

	//float to int32 trunc
	{
		constexpr float testArray[] = {1.00,2.58, 22523.225, -1431.252, 0.002, 125.2, 938.0, 995.0};
		constexpr int32_t correctArray[] = {1,2,22523, -1431, 0, 125, 938, 995};

		int32_t loadArray[8];

		testTrunc(testArray, correctArray, loadArray, "Error Float to Int32 Trunc!");
	}
}

TEST(AvxTest, AddTest)
{
	auto testAdd = [](const auto& testArray1, const auto& testArray2, const auto& correctArray, auto& loadArray, const std::string& _Error_Msg = "", double _Epsilon = 0.0001)
	{
		auto loadData1 = load<false>(testArray1);
		auto loadData2 = load<false>(testArray2);

		typedef std::decay_t<decltype(testArray1[0])> Src_Element_Tp;
		typedef std::decay_t<decltype(testArray2[0])> Dst_Element_Tp;
		assert(std::is_same_v<Src_Element_Tp, Dst_Element_Tp>);
		constexpr bool IntToIntAdd = vmath_concept::Int_Type<Src_Element_Tp> && vmath_concept::Int_Type<Dst_Element_Tp>;

		if constexpr (IntToIntAdd)
		{
			auto storeData = add<sizeof(Src_Element_Tp)*8>(loadData1, loadData2);
			store<false>(loadArray, storeData);
		}
		else 
		{
			auto storeData = add(loadData1, loadData2);
			store<false>(loadArray, storeData);
		}

		if constexpr (IntToIntAdd)
		{
			EXPECT_THAT(loadArray, ElementsAreArray(correctArray)) << _Error_Msg.c_str();
		}
		else
		{
			EXPECT_THAT(loadArray, Pointwise(DoubleNear(_Epsilon), correctArray)) << _Error_Msg.c_str();
		}
	};

	//double add
	{
		constexpr double testArray1[] = {1.00,2.58, 22523.225, -1431.252};
		constexpr double testArray2[] = {6.38,2.90, 32.32, 1152.235};
		constexpr double correctArray[] = {7.38, 5.48, 22555.545, -278.999};

		double loadArray[4];

		testAdd(testArray1, testArray2, correctArray, loadArray, "Error Double Add!");
	}

	//float add
	{
		constexpr float testArray1[] = {1.00,2.58, 22523.225, -1431.252, 0.000, 0.001, -135151.1, -12345.2};
		constexpr float testArray2[] = {6.38,2.90, 32.32, 1152.235, -0.001, 0.002, 22432.5, 22222.0};
		constexpr float correctArray[] = {7.38, 5.48, 22555.545, -278.999, -0.001, 0.003, -112718.6, 9876.8};

		float loadArray[8];

		testAdd(testArray1, testArray2, correctArray, loadArray, "Error Float Add!");
	}

	//int_8 add
	{
		constexpr int8_t testArray1[] = 
		{
			44, 76, -87, -27, 
			-48, 38, 82, 63, 
			-46, -39, 56, 95, 
			-59, -49, 10, -27,
			-24, 3, -3, -63, 
			59, -84, -71, 69, 
			47, -45, 98, -59, 
			11, 1, 42, -44
		};
		constexpr int8_t testArray2[] = 
		{
			-7, 9, 20, 12, 
			-20, -20, 24, 1, 
			5, 15, 19, 16, 
			13, -7, 6, -23,
			-16, 21, -28, -1, 
			-4, 16, 25, -23, 
			1, 27, 20, 3, 
			-2, -12, 6, -15
		};
		constexpr int8_t correctArray[] = 
		{
			37, 85, -67, -15, 
			-68, 18, 106, 64, 
			-41, -24, 75, 111, 
			-46, -56, 16, -50,
			-40, 24, -31, -64, 
			55, -68, -46, 46, 
			48, -18, 118, -56, 
			9, -11, 48, -59
		};

		int8_t loadArray[32];

		testAdd(testArray1, testArray2, correctArray, loadArray, "Error int8 Add!");
	}

	//int_16 add
	{

		constexpr int16_t testArray1[] = 
		{
			-12845, -2894, 949, -1583, 
			-1205, 7850, 10614, -7178, 
			10963, 4734, -3646, 7152, 
			-4962, 11657, -3974, -12952
		};
		constexpr int16_t testArray2[] = 
		{
			13897, -11485, -8601, -6040, 
			-14227, 10709, 2863, -472, 
			12279, 2100, 2564, -2283, 
			-4332, -7854, 1974, 9118
		};
		constexpr int16_t correctArray[] = 
		{
			1052, -14379, -7652, -7623, 
			-15432, 18559, 13477, -7650, 
			23242, 6834, -1082, 4869, 
			-9294, 3803, -2000, -3834
		};

		int16_t loadArray[16];

		testAdd(testArray1, testArray2, correctArray, loadArray, "Error int16 Add!");
	}

	//int_32 add
	{
		constexpr int32_t testArray1[] = 
		{
			-240677943, 387001464, -328414502, 356474935, 
			8006305, -105365053, -59408779, -384227500
		};
		constexpr int32_t testArray2[] = 
		{
			336596329, -464730860, -17983896, -159551385, 
			-320373709, 193520910, 23487959, 163383611
		};
		constexpr int32_t correctArray[] = 
		{
			95918386, -77729396, -346398398, 196923550, 
			-312367404, 88155857, -35920820, -220843889
		};

		int32_t loadArray[8];

		testAdd(testArray1, testArray2, correctArray, loadArray, "Error int32 Add!");
	}

	//int_64 add
	{
		constexpr int64_t testArray1[] = 
		{
			-146986939035, -201793000188, 357865020, 256172242
		};
		constexpr int64_t testArray2[] = 
		{
			91139323045, 313860776200, 320829791, -386687719
		};
		constexpr int64_t correctArray[] = 
		{
			-55847616010, 112067776012, 678694811, -130515477
		};

		int64_t loadArray[4];

		testAdd(testArray1, testArray2, correctArray, loadArray, "Error int64 Add!");
	}
}

TEST(AvxTest, SubTest)
{
	auto testSub = [](const auto& testArray1, const auto& testArray2, const auto& correctArray, auto& loadArray, const std::string& _Error_Msg = "", double _Epsilon = 0.0001)
	{
		auto loadData1 = load<false>(testArray1);
		auto loadData2 = load<false>(testArray2);

		typedef std::decay_t<decltype(testArray1[0])> Src_Element_Tp;
		typedef std::decay_t<decltype(testArray2[0])> Dst_Element_Tp;
		assert(std::is_same_v<Src_Element_Tp, Dst_Element_Tp>);
		constexpr bool IntToIntSub = vmath_concept::Int_Type<Src_Element_Tp> && vmath_concept::Int_Type<Dst_Element_Tp>;

		if constexpr (IntToIntSub)
		{
			auto storeData = sub<sizeof(Src_Element_Tp)*8>(loadData1, loadData2);
			store<false>(loadArray, storeData);
		}
		else 
		{
			auto storeData = sub(loadData1, loadData2);
			store<false>(loadArray, storeData);
		}

		if constexpr (IntToIntSub)
		{
			EXPECT_THAT(loadArray, ElementsAreArray(correctArray)) << _Error_Msg.c_str();
		}
		else
		{
			EXPECT_THAT(loadArray, Pointwise(DoubleNear(_Epsilon), correctArray)) << _Error_Msg.c_str();
		}
	};

	//double sub
	{
		constexpr double testArray1[] = {1.00,2.58, 22523.225, -1431.252};
		constexpr double testArray2[] = {6.38,2.90, 32.32, 1152.235};
		constexpr double correctArray[] = {-5.38, -0.32, 22490.905, -2583,487};

		double loadArray[4];

		testSub(testArray1, testArray2, correctArray, loadArray, "Error Double Sub!");
	}

	//float sub
	{
		constexpr float testArray1[] = {1.00,2.58, 22523.225, -1431.252, 0.000, 0.001, -135151.1, -12345.2};
		constexpr float testArray2[] = {6.38,2.90, 32.32, 1152.235, -0.001, 0.002, 22432.5, 22222.0};
		constexpr float correctArray[] = {-5.38, -0.32, 22490.905, -2583,487, 0.001, -0.001, -157583.6, -34567.2};

		float loadArray[8];

		testSub(testArray1, testArray2, correctArray, loadArray, "Error Float Sub!");
	}

	//int_8 sub
	{
		constexpr int8_t testArray1[] = 
		{
			44, 76, -87, -27, 
			-48, 38, 82, 63, 
			-46, -39, 56, 95, 
			-59, -49, 10, -27,
			-24, 3, -3, -63, 
			59, -84, -71, 69, 
			47, -45, 98, -59, 
			11, 1, 42, -44
		};
		constexpr int8_t testArray2[] = 
		{
			-7, 9, 20, 12, 
			-20, -20, 24, 1, 
			5, 15, 19, 16, 
			13, -7, 6, -23,
			-16, 21, -28, -1, 
			-4, 16, 25, -23, 
			1, 27, 20, 3, 
			-2, -12, 6, -15
		};
		constexpr int8_t correctArray[] = 
		{
			51, 67, -107, -39, 
			-28, 58, 58, 62, 
			-51, -54, 37, 79, 
			-72, -42, 4, -4, 
			-8, -18, 25, -62, 
			63, -100, -96, 92, 
			46, -72, 78, -62, 
			13, 13, 36, -29
		};

		int8_t loadArray[32];

		testSub(testArray1, testArray2, correctArray, loadArray, "Error int8 Sub!");
	}

	//int_16 sub
	{

		constexpr int16_t testArray1[] = 
		{
			-12845, -2894, 949, -1583, 
			-1205, 7850, 10614, -7178, 
			10963, 4734, -3646, 7152, 
			-4962, 11657, -3974, -12952
		};
		constexpr int16_t testArray2[] = 
		{
			13897, -11485, -8601, -6040, 
			-14227, 10709, 2863, -472, 
			12279, 2100, 2564, -2283, 
			-4332, -7854, 1974, 9118
		};
		constexpr int16_t correctArray[] = 
		{
			-26742, 8591, 9550, 4457, 
			13022, -2859, 7751, -6706, 
			-1316, 2634, -6210, 9435, 
			-630, 19511, -5948, -22070
		};

		int16_t loadArray[16];

		testSub(testArray1, testArray2, correctArray, loadArray, "Error int16 Sub!");
	}

	//int_32 sub
	{
		constexpr int32_t testArray1[] = 
		{
			-240677943, 387001464, -328414502, 356474935, 
			8006305, -105365053, -59408779, -384227500
		};
		constexpr int32_t testArray2[] = 
		{
			336596329, -464730860, -17983896, -159551385, 
			-320373709, 193520910, 23487959, 163383611
		};
		constexpr int32_t correctArray[] = 
		{
			-577274272, 851732324, -310430606, 516026320, 
			328380014, -298885963, -82896738, -547611111
		};

		int32_t loadArray[8];

		testSub(testArray1, testArray2, correctArray, loadArray, "Error int32 Sub!");
	}

	//int_64 sub
	{
		constexpr int64_t testArray1[] = 
		{
			-146986939035, -201793000188, 357865020, 256172242
		};
		constexpr int64_t testArray2[] = 
		{
			91139323045, 313860776200, 320829791, -386687719
		};
		constexpr int64_t correctArray[] = 
		{
			-238126262080, -515653776388, 37035229, 642859961
		};

		int64_t loadArray[4];

		testSub(testArray1, testArray2, correctArray, loadArray, "Error int64 Sub!");
	}
}

TEST(AvxTest, MulTest)
{
	auto testMul = [](const auto& testArray1, const auto& testArray2, const auto& correctArray, auto& loadArray, const bool aligned = false, const std::string& _Error_Msg = "", double _Epsilon = 0.0001)
	{
		auto loadData1 = aligned ? load<true>(testArray1) : load<false>(testArray1);
		auto loadData2 = aligned ? load<true>(testArray2) : load<false>(testArray2);

		typedef std::decay_t<decltype(testArray1[0])> Src_Element_Tp;
		typedef std::decay_t<decltype(testArray2[0])> Dst_Element_Tp;
		assert(std::is_same_v<Src_Element_Tp, Dst_Element_Tp>);
		constexpr bool IntToIntMul = vmath_concept::Int_Type<Src_Element_Tp> && vmath_concept::Int_Type<Dst_Element_Tp>;

		if constexpr (IntToIntMul)
		{
			if(aligned){
				auto storeData = mul<32, true>(loadData1, loadData2);
				store<true>(loadArray, storeData);
			}
			else{
				auto storeData = mul<32, false>(loadData1, loadData2);
				store<false>(loadArray, storeData);
			}
		}
		else 
		{
			auto storeData = mul(loadData1, loadData2);
			store<false>(loadArray, storeData);
		}

		if constexpr (IntToIntMul)
		{
			EXPECT_THAT(loadArray, ElementsAreArray(correctArray)) << _Error_Msg.c_str();
		}
		else
		{
			EXPECT_THAT(loadArray, Pointwise(DoubleNear(_Epsilon), correctArray)) << _Error_Msg.c_str();
		}
	};

	//double mul
	{
		constexpr double testArray1[] = {1.00,2.58, 22523.225, -1431.252};
		constexpr double testArray2[] = {6.38,2.90, 32.32, 1152.235};
		constexpr double correctArray[] = {6.38, 7.482, 727950.632, -1649138.64822};

		double loadArray[4];

		testMul(testArray1, testArray2, correctArray, loadArray, "Error Double Mul!");
	}

	//float mul
	{
		constexpr float testArray1[] = {1.00,2.58, 22523.225, -1431.252, 0.000, 0.001, -135151.1, -12345.2};
		constexpr float testArray2[] = {6.38,2.90, 32.32, 1152.235, -0.001, 0.002, 22432.5, 22222.0};
		constexpr float correctArray[] = {6.38, 7.482, 727950.632, -1649138.64822, 0.0, 2.0E-6, -3.03177705E9, -2.743350344E8 };

		float loadArray[8];

		testMul(testArray1, testArray2, correctArray, loadArray, "Error Float Mul!");
	}

	//int_32 mul aligned
	{
		alignas(32) constexpr int32_t testArray1[] = 
		{
			-240677943, 387001464, -328414502, 356474935, 
			8006305, -105365053, -59408779, -384227500
		};
		alignas(32) constexpr int32_t testArray2[] = 
		{
			336596329, -464730860, -17983896, -159551385, 
			-320373709, 193520910, 23487959, 0
		};
		constexpr int32_t correctArray[] = 
		{
			274827889, 405786976, -488627056, 460506657, 
			675510803, -880867414, 959405379, 0
		};

		int32_t loadArray[8];

		testMul(testArray1, testArray2, correctArray, loadArray, aligned = true, _Error_Msg = "Error int32 Aligned Mul!");
	}

	//int_32 mul unaligned
	{
		constexpr int32_t testArray1[] = 
		{
			-240677943, 387001464, -328414502, 356474935, 
			8006305, -105365053, -59408779, -384227500
		};
		constexpr int32_t testArray2[] = 
		{
			336596329, -464730860, -17983896, -159551385, 
			-320373709, 193520910, 23487959, 0
		};
		constexpr int32_t correctArray[] = 
		{
			274827889, 405786976, -488627056, 460506657, 
			675510803, -880867414, 959405379, 0
		};

		int32_t loadArray[8];

		testMul(testArray1, testArray2, correctArray, loadArray, "Error int32 Unaligned Mul!");
	}
}

TEST(AvxTest, divTest)
{
	auto testDiv = [](const auto& testArray1, const auto& testArray2, const auto& correctArray, auto& loadArray, const std::string& _Error_Msg = "", double _Epsilon = 0.0001)
	{
		auto loadData1 = load<false>(testArray1);
		auto loadData2 = load<false>(testArray2);

		typedef std::decay_t<decltype(testArray1[0])> Src_Element_Tp;
		typedef std::decay_t<decltype(testArray2[0])> Dst_Element_Tp;
		assert(std::is_same_v<Src_Element_Tp, Dst_Element_Tp>);
		
		auto storeData = div(loadData1, loadData2);
		store<false>(loadArray, storeData);

		EXPECT_THAT(loadArray, Pointwise(DoubleNear(_Epsilon), correctArray)) << _Error_Msg.c_str();
	};

	//double div
	{
		constexpr double testArray1[] = {1.00,2.58, 22523.225, -1431.252};
		constexpr double testArray2[] = {6.38,2.90, 32.32, 1152.235};
		constexpr double correctArray[] = {0.1567398119, 0.88965517, 696.881961634, -1.242152860};

		double loadArray[4];

		testDiv(testArray1, testArray2, correctArray, loadArray, "Error Double Div!");
	}

	//float div
	{
		constexpr float testArray1[] = {1.00,2.58, 22523.225, -1431.252, 0.000, 0.001, -135151.1, -12345.2};
		constexpr float testArray2[] = {6.38,2.90, 32.32, 1152.235, -0.001, 0.002, 22432.5, 22222.0};
		constexpr float correctArray[] = {0.1567398119, 0.88965517, 696.881961634, -1.242152860, 0.0, 0.5,-6.02478993, -0.55553956};

		float loadArray[8];

		testDiv(testArray1, testArray2, correctArray, loadArray, "Error Float Div!");
	}
}




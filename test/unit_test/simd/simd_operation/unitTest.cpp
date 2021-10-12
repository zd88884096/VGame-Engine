#include "simd/simd_operation.h"
#include "timer.h"
#include <iostream>
using namespace vmath_test;



int main()
{
	Timer timer;
	constexpr int testEpoch = 10000000;
	float x[] = { 0, 0, 0, 0, 3, 4.4, 5.1, 6 };
	__m256d z;
	double y[4];
	timer.millisecondsElapsedf();
	for (int i = 0; i < testEpoch; ++i)
	{
		__m128 w = vmath_sse::load<false>(x);
		vmath_avx::cast_impl(z, w);
		vmath_avx::store<false>(y, z);
	}
	auto time = timer.millisecondsElapsedf();
	std::cout << "Custom Convert Time Needed: " << time<<std::endl;

	timer.millisecondsElapsedf();
	for (int i = 0; i < testEpoch; ++i)
	{
		y[0] = x[4];
		y[1] = x[5];
		y[2] = x[6];
		y[3] = x[7];
	}
	time = timer.millisecondsElapsedf();
	std::cout << "System Convert Time Needed: " << time << std::endl;


	return 0;
}

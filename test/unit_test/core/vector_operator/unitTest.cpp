#include "core/vector_operator.h"
#include "timer.h"
#include <iostream>

using namespace std;
using namespace vmath_test;
using namespace vmath_vector_operator;


int main()
{
	int x = 1; 
	double w = 1.2;

	Timer timer;
	constexpr int testEpoch = 100000000;
	auto time = timer.millisecondsElapsedf();
	{
		int x = 1;
		for (int i = 0; i < testEpoch; ++i)
		{
			auto y = getOperator<vectorAdd>(x, 1);
			auto z = getOperator<vectorAdd>(w, 1);
			auto q = y + z;
		}

	}
	time = timer.millisecondsElapsedf();
	std::cout << "Vector Sum Operator Construction Time: " << time << std::endl;


	
}
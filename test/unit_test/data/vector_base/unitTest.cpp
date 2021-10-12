#include "timer.h"
#include "data/vector_base.h"
#include "core/vector_operator.h"
#include <iostream>

using namespace std;
using namespace vmath_test;
using namespace vmath_vector_base;

int main()
{
	Timer timer;
	constexpr int testEpoch = 100000;

	{
		int x = 1, y = 2;
		vmath_vector_operator::vectorAddOperator z(x, 1);
		x = 2;
		cout << z.lhs() << endl;

	}
	
	

}

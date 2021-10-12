#include "data/vector.h"
#include "core/vector_evaluator.h"
#include "timer.h"
#include <iostream>

using namespace std;
using namespace vmath;
using namespace vmath_test;
using namespace vmath_vector_evaluator;
using namespace vmath_vector_operator;
typedef Vector<float, 4> vt;

Timer timer;
constexpr auto testEpoch = 10000000;

void runCustom()
{
	Vector<float, 16> a, b;
	Vector<float, 16> d;
	d[0] = 1.0;
	d[1] = 2.2;
	d[2] = 3.2;
	d[3] = 4.0;
	a = d;
	timer.record();
	for (int i = 0; i < testEpoch; ++i) {
		d = a + d + d + a;
	}
	auto time = timer.millisecondsElapsedf();
	cout << "VMath Add Time: " << time << endl;
}


void runtimeTest()
{
	runCustom();

}

int main()
{
	runtimeTest();
	
}

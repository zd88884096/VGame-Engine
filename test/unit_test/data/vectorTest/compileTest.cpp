#include "../../../../include/data/vector.h"
#include "../../../utility/timer.h"
#include <glm/glm.hpp>
#include <iostream>
#include <random>
#include <time.h>
#include <vector>
using namespace vmath;
using namespace std;
using namespace vmath_test;


template<typename Tp>
vector<Tp> operator+(const vector<Tp>& a, const vector<Tp>& b)
{
	vector<Tp> ret(a.size());
	for (size_t i = 0; i < a.size(); ++i)
	{
		ret[i] = a[i] + b[i];
	}
	return ret;
}

void runtimeTest()
{
	std::srand(time(NULL));
	Timer timer;

	int testEpoch = 1000000;
	const int vectorLength = 4;
	{
		//std::vector<float> a(vectorLength), b(vectorLength);
		glm::vec4 a, b;
		for (int i = 0; i < vectorLength; ++i)
		{
			a[i] = rand();
			b[i] = rand();
		}
		timer.millisecondsElapsedf();
		// Test std vector
		for (int i = 0; i < testEpoch; ++i)
		{
			a =  a + a + a + a+ a + b;
		}
		float elapsedTime = timer.millisecondsElapsedf();
		cout << "glm Time Elapsed: " << elapsedTime << endl;
	}

	{
		Vector<float, vectorLength> a, b;
		Vector<float, DYNAMIC_SIZE> c(size_t(4));
		timer.millisecondsElapsedf();
		for (int i = 0; i < vectorLength; ++i)
		{
			a[i] = rand();
			b[i] = rand();
		}
		// Test costume vector
		for (int i = 0; i < 10; ++i)
		{
			 auto c = a + a + a + a + a + a;
		}
		auto elapsedTime = timer.millisecondsElapsedf();
		cout << "vmath::Vector Time Elapsed: " << elapsedTime << endl;
	}
}
int main()
{
	using namespace vmath::detail;
	runtimeTest();
	

	return 0;
}

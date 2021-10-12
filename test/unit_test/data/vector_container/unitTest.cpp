#include "data/vector_container.h"
#include "timer.h"
#include <iostream>
#include "glm/glm.hpp"
#include <eigen3/Eigen/Core>
#include <vector>
#include "core/vector_operator.h"

using namespace std;
using namespace vmath_test;
using namespace vmath_vector_container;

using Eigen::Vector4f;

template<typename T, size_t Size>
void print(vectorContainer<T, Size>& v)
{
	for (int i = 0; i < v.size(); ++i)
	{
		cout << v[i] << " ";
	}
	puts("");
}

struct op
{
	int& x, &y;
	op(int& _x, int& _y) :x(_x), y(_y)
	{

	}
};

struct op1
{
	std::tuple<int&, int&> tp;
	op1(int& _x, int& _y) : tp(_x, _y)
	{

	}
};

int main()
{
	Timer timer;
	auto time = timer.millisecondsElapsedf();
	constexpr int testEpoch = 100000000;
	{
		

		timer.millisecondsElapsedf();
		int a = 0, b = 0;
		auto w = vmath_vector_operator::getOperator<vmath_vector_operator::vectorAdd>(a, 1);
		std::cout << w.lhs() << " " << w.rhs() << std::endl;
		a = 2, b = 3;
		std::cout << w.lhs() << " " << w.rhs() << std::endl;
		std::cout << sizeof(w) << std::endl;
		auto y = w + w;
		std::cout << sizeof(y) << std::endl;
		for (int i = 0; i < testEpoch; ++i)
		{
			auto w = vmath_vector_operator::getOperator<vmath_vector_operator::vectorAdd>(a, b);	
		}
		time = timer.millisecondsElapsedf();
		cout << "Custom Add Time Elapsed: " << time << endl;
	}
	{
		glm::f32vec4 x;
		glm::i32vec4 y, z, w, k;
	
		timer.millisecondsElapsedf();
		for (int i = 0; i < testEpoch; ++i)
		{
			 w = z + y;
		}
		time = timer.millisecondsElapsedf();

		cout << "GLM Add Time Elapsed: " << time << endl;
	}


	{
		Vector4f x, y;

		timer.millisecondsElapsedf();
		for (int i = 0; i < testEpoch; ++i)
		{
			auto z = x + y;
		}
		time = timer.millisecondsElapsedf();

		cout << "Eigen Add Time Elapsed: " << time << endl;
	}

	

	return 0;
}
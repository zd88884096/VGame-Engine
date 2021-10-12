#pragma once
#include "../common/common.h"

namespace vmath_tuple_container
{
	/*
		This part defines a series of custom tuples that accelerate the speed of operator construction	
	*/

	template<typename ...Tps>
	class Tuple;

	template<typename Tp1>
	class Tuple<Tp1>
	{
	protected:
		Tp1 m_X1;
	public:
		Tuple(Tp1 _x1) : m_X1(_x1)
		{

		}

		template<size_t index>
		const auto& get() const;

		template<>
		const auto& get<0>() const
		{
			return m_X1;
		}
	};

	template<typename Tp1, typename Tp2>
	class Tuple<Tp1, Tp2>
	{
	protected:
		Tp1 m_X1;
		Tp2 m_X2;
	public:
		Tuple(Tp1 _x1, Tp2 _x2)
			: m_X1(_x1), m_X2(_x2)
		{

		}
		
		template<size_t index>
		const auto& get() const;

		template<>
		const auto& get<0>() const
		{
			return m_X1;
		}

		template<>
		const auto& get<1>() const
		{
			return m_X2;
		}
	};

}
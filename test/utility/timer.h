#pragma once
#include <thread>
#include <chrono>
#include <type_traits>
#include <ctime>
#include <mutex>
#include <concepts>


namespace vmath_test {
	namespace detail {
		typedef std::chrono::seconds Timer_Second_Type;
		typedef std::chrono::milliseconds Timer_Millisecond_Type;
		typedef std::chrono::microseconds Timer_Microsecond_Type;
	}
	template<typename _Time_Tp>
	concept timeType = std::is_same<detail::Timer_Second_Type, _Time_Tp>::value ||
		std::is_same<detail::Timer_Millisecond_Type, _Time_Tp>::value ||
		std::is_same<detail::Timer_Microsecond_Type, _Time_Tp>::value;

	class Timer {
	private:
		auto update()
		{
			std::lock_guard<std::mutex> mutexGuard(m_Mutex);
			auto now = std::chrono::steady_clock::now();
			auto delta = now - m_LastTime;
			m_LastTime = now;
			return delta;
		}
		detail::Timer_Second_Type secondsElapsed() {
				return std::chrono::duration_cast<detail::Timer_Second_Type>(update());
			};

			detail::Timer_Millisecond_Type millisecondsElapsed() {
				return std::chrono::duration_cast<detail::Timer_Millisecond_Type>(update());
			};

			detail::Timer_Microsecond_Type microsecondsElapsed() {
				return std::chrono::duration_cast<detail::Timer_Microsecond_Type>(update());
			}

	public:
		Timer() = default;
		~Timer() = default;

		template<timeType _Dst_Tp, timeType _Src_Tp>
		static _Dst_Tp converTime(const _Src_Tp& src) {
			return std::chrono::duration_cast<_Dst_Tp>(src);
		};
		
		void record()
		{
			m_LastTime = std::chrono::steady_clock::now();
		}
	
		float secondsElapsedf() {
			return static_cast<float>(secondsElapsed().count());
		}

		float millisecondsElapsedf() {
			return static_cast<float>(millisecondsElapsed().count());
		}

		float microsecondsElapsedf() {
			return static_cast<float>(microsecondsElapsed().count());
		}

	private:
		std::mutex m_Mutex;
		std::chrono::steady_clock::time_point m_LastTime;
	};

}



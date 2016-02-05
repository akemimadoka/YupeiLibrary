#pragma once

#include <chrono>

namespace Yupei
{
	class StopWatch
	{
	public:
		void Start()
		{
			startTime = std::chrono::high_resolution_clock::now();
		}

		//nano second_obj
		auto Stop()
		{
			auto endTime = std::chrono::high_resolution_clock::now();
			return (endTime - startTime).count();
		}
	private:
		std::chrono::high_resolution_clock::time_point startTime;
	};
}
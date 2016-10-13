#pragma once

#include <chrono>

namespace Yupei
{
	class StopWatch
	{
	public:
		void Start() noexcept
		{
			startTime_ = std::chrono::high_resolution_clock::now();
		}

		auto Stop() noexcept
		{
			return (std::chrono::high_resolution_clock::now() - startTime_).count();
		}

	private:
		std::chrono::high_resolution_clock::time_point startTime_;
	};
}
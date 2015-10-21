#pragma once

#include "utility_internal.h"

namespace Yupei
{
	template<typename ObjectT, typename... Args>
	void construct(ObjectT* ptr, Args&&... args)
	{
		new (static_cast<void*>(ptr)) ObjectT(Yupei::forward<Args>(args)...);
	}

	template<typename ObjectT>
	void destroy(ObjectT* ptr) noexcept
	{
		ptr->~ObjectT();
	}

}



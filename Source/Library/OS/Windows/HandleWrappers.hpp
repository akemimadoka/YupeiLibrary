#pragma once

#include "..\..\HandleWrapper.hpp"
#include "Win32Wrappers.hpp"

namespace Yupei
{
    struct FileHandleCloser
    {
        using HandleType = NativeWindowHandle;
		const NativeWindowHandle InvalidHandle = GetInvalidWindowHandle();

        void operator()(NativeWindowHandle handle) noexcept
        {
            (void)CloseHandleWrapper(handle);
        }
    };

    using FileHandle = HandleWrapper<FileHandleCloser>;

	struct LocalMemoryHandleCloser
	{
		using HandleType = NativeWindowHandle;
		static constexpr NativeWindowHandle InvalidHandle = {};

		void operator()(NativeWindowHandle handle) noexcept
		{
			(void)LocalFreeWrapper(handle);
		}
	};

	using LocalMemoryHandle = HandleWrapper<LocalMemoryHandleCloser>;

	struct DllHandleCloser
	{
		using HandleType = ModuleHandle;
		static constexpr ModuleHandle InvalidHandle = {};

		void operator()(ModuleHandle handle) noexcept
		{
			(void)FreeLibraryWrapper(handle);
		}
	};

	using DllHandle = HandleWrapper<DllHandleCloser>;
}
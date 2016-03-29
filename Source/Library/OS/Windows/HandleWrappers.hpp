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
}
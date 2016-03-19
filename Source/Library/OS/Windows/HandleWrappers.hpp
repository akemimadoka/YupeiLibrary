#pragma once

#include "..\..\HandleWrapper.hpp"
#include "Win32Wrappers.hpp"

namespace Yupei
{
    struct FileHandleCloser
    {
        using HandleType = NativeWindowHandle;
        static constexpr HandleType InvalidHandle = InvalidWindowHandleValue;

        void operator()(NativeWindowHandle handle) noexcept
        {
            (void)CloseHandleWrapper(handle);
        }
    };

    extern template class HandleWrapper<FileHandleCloser>;

    using FileHandle = HandleWrapper<FileHandleCloser>;
}
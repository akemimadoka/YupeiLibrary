#pragma once

#include "../../HandleWrapper.hpp"
#include "Win32Wrappers.hpp"

namespace Yupei
{
	struct FileHandleCloser
	{
		using HandleType = NativeHandle;
		static const NativeHandle InvalidHandle;

		void operator()(NativeHandle handle) noexcept;
	};

	using FileHandle = HandleWrapper<FileHandleCloser>;

	struct LocalMemoryHandleCloser
	{
		using HandleType = NativeHandle;
		static constexpr NativeHandle InvalidHandle = {};

		void operator()(NativeHandle handle) noexcept;
	};

	using LocalMemoryHandle = HandleWrapper<LocalMemoryHandleCloser>;

	struct NativeModuleHandleCloser
	{
		using HandleType = NativeModuleHandle;
		static constexpr NativeModuleHandle InvalidHandle = {};

		void operator()(NativeModuleHandle handle) noexcept;
	};

	using ModuleHandle = HandleWrapper<NativeModuleHandleCloser>;

	struct RegKeyCloser
	{
		using HandleType = NativeKeyHandle;
		//MSDN 没有提到非法值。暂定为 nullptr。
		static constexpr HandleType InvalidHandle = {};

		void operator()(HandleType handle) noexcept;
	};

	using RegKeyHandle = HandleWrapper<RegKeyCloser>;
}
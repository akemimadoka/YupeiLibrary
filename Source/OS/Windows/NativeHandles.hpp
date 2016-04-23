#pragma once

#include "..\..\HandleWrapper.hpp"
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

	struct DllHandleCloser
	{
		using HandleType = ModuleHandle;
		static constexpr ModuleHandle InvalidHandle = {};

		void operator()(ModuleHandle handle) noexcept;
	};

	using DllHandle = HandleWrapper<DllHandleCloser>;

	struct SocketCloser
	{
		using HandleType = std::uintptr_t;
		static constexpr HandleType InvalidHandle = static_cast<std::uintptr_t>(-1);

		void operator()(HandleType handle) noexcept;
	};

	using Socket = HandleWrapper<SocketCloser>;

	struct WsaEventCloser
	{
		using HandleType = NativeHandle;
		static constexpr HandleType InvalidHandle = {};

		void operator()(HandleType handle) noexcept;
	};

	using WsaEvent = HandleWrapper<WsaEventCloser>;

	struct RegKeyCloser
	{
		using HandleType = KeyHandle;
		//MSDN 没有提到非法值。暂定为 nullptr。
		static constexpr HandleType InvalidHandle = {};

		void operator()(HandleType handle) noexcept;
	};

	using RegKeyHandle = HandleWrapper<RegKeyCloser>;
}
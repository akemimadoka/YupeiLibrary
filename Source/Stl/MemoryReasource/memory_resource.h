#pragma once

#include <cstddef>
#include <cstdint>
#include "..\..\Custom\Extensions.h"

namespace Yupei
{
	class memory_resource
	{
		static constexpr std::size_t max_align = alignof(std::max_align_t);
		
	public:
		
		using size_type = std::size_t;

		virtual ~memory_resource();

		void* allocate(size_type bytes, size_type alignment = max_align);

		void deallocate(void* p, size_type bytes, size_type alignment = max_align);

		bool is_equal(const memory_resource& other) const noexcept;

	protected:

		virtual void* do_allocate(size_type bytes, size_type alignment) = 0;

		virtual void do_deallocate(void* p, size_type bytes, size_type alignment) = 0;

		virtual bool do_is_equal(const memory_resource& other) const noexcept = 0;

	};

	inline bool operator == (const memory_resource& lhs, const memory_resource& rhs) noexcept
	{
		return &lhs == &rhs || lhs.is_equal(rhs);
	}

	inline bool operator != (const memory_resource& lhs, const memory_resource& rhs) noexcept
	{
		return !(lhs == rhs);
	}

	extern memory_resource* new_delete_resource() noexcept;

	extern memory_resource* null_memory_resource() noexcept;

	extern memory_resource* set_default_resource(memory_resource* r) noexcept;

	extern memory_resource* get_default_resource() noexcept;

	//http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0148r0.pdf

	class memory_resource_ptr
	{
	private:
		memory_resource* resource_;

	public:
		memory_resource_ptr() noexcept
			: resource_(get_default_resource()) 
		{}

		memory_resource_ptr(std::nullptr_t) noexcept
			: resource_(get_default_resource()) 
		{}

		explicit memory_resource_ptr(memory_resource* p) noexcept
			: resource_(p ? p : get_default_resource()) 
		{}

		memory_resource_ptr(const memory_resource_ptr&) noexcept = default;

		 ~memory_resource_ptr() = default;

		memory_resource& operator=(const memory_resource_ptr&) = delete;

		void reset(memory_resource* p) noexcept
		{
			resource_ = p ? p : get_default_resource();
		}

		void reset(std::nullptr_t) noexcept
		{
			resource_ = get_default_resource();
		}

		void reset(memory_resource_ptr p) noexcept
		{
			resource_ = p.resource_;
		}

		memory_resource* get() const noexcept
		{
			return resource_;
		}

		memory_resource* operator->() const noexcept
		{
			return resource_;
		}

		memory_resource& operator*() const noexcept
		{
			return *resource_;
		}
	};

	inline bool operator==(memory_resource_ptr a, memory_resource_ptr b) 
	{ 
		return a.get() == b.get(); 
	}

	inline bool operator!=(memory_resource_ptr a, memory_resource_ptr b) 
	{ 
		return a.get() != b.get(); 
	}

	
	template<typename T>
	class polymorphic_allocator
	{
	public:

		using value_type = T;
		using size_type = std::size_t;

		polymorphic_allocator() noexcept = default;

		polymorphic_allocator(memory_resource_ptr r)
			:resource_{r}
		{
		}

		DEFAULTCOPY(polymorphic_allocator)

		value_type* allocate(size_type n)
		{
			return static_cast<value_type*>(resource_->allocate(n * sizeof(value_type), alignof(value_type)));
		}

		void deallocate(value_type* p, size_type n)
		{
			resource_->deallocate(p, n * sizeof(value_type), alignof(value_type));
		}

		template <typename U>
		polymorphic_allocator(const polymorphic_allocator<U>& other) noexcept
			:resource_{other.resource()}
		{
		}

		memory_resource_ptr resource() const noexcept
		{
			return resource_;
		}
	private:
		memory_resource_ptr resource_;
	};

	template <typename T1, typename T2>
	inline bool operator == (const polymorphic_allocator<T1>& a, const polymorphic_allocator<T2>& b) noexcept
	{
		return *a.resource() == *b.resource();
	}

	template <typename T1, typename T2>
	inline bool operator != (const polymorphic_allocator<T1>& a, const polymorphic_allocator<T2>& b) noexcept
	{
		return !(a == b);
	}


	namespace Internal
	{
		constexpr inline memory_resource::size_type GetFinalSize(memory_resource::size_type bytes, memory_resource::size_type alignment = alignof(std::max_align_t))
		{
			return (bytes + alignment - 1) & ~(alignment - 1);
		}

		inline std::size_t GetFinalOffset(const void* addr, std::size_t alignment)
		{
			const auto mask = alignment - 1;

			return mask - ((static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(addr)) - 1) & mask);
		}

		class BufferManager
		{
		private:
			using ByteType = unsigned char;

		public:
			using size_type = std::size_t;

			BufferManager(void* buffer, size_type bufferSize)
				:buffer_{static_cast<ByteType*>(buffer)},
				bufferSize_{bufferSize},
				cursor_{}
			{
			}

			BufferManager() noexcept
				:buffer_{}, bufferSize_{}, cursor_{}
			{}

			void* Allocate(size_type size, size_type alignment);

			ByteType* ReplaceBuffer(void* newBuffer, size_type newBufferSize);

			void Release() noexcept
			{
				cursor_ = {};
			}

			void Reset() noexcept
			{
				buffer_ = {};
				bufferSize_ = {};
				cursor_ = {};
			}

			void Deallocate(void* ptr, size_type size, size_type alignment)
			{
				//no-op
			}

			ByteType* GetBuffer() const noexcept
			{
				return buffer_;
			}

			size_type GetBufferSize() const noexcept
			{
				return bufferSize_;
			}

			DISABLECOPY(BufferManager)

		private:

			ByteType* buffer_;
			size_type bufferSize_;
			size_type cursor_;

		};

		class SimplePoolManager
		{		
		public:
			using size_type = std::size_t;

		private:
			struct alignas(std::max_align_t) Block
			{
				Block* nextBlock_;
				size_type blockSize_;
			};
		public:

			SimplePoolManager() noexcept
				:headBlock_{} {}

			SimplePoolManager(memory_resource_ptr upstream)
				:headBlock_{}, upstream_{ upstream } {}

			void* Allocate(size_type size, size_type alignment);

			void Deallocate(void* ptr, size_type size, size_type alignment)
			{
				//no-op
			}

			void Release();

			~SimplePoolManager()
			{
				Release();
			}

			DISABLECOPY(SimplePoolManager)

		private:

			Block* headBlock_;
			memory_resource_ptr upstream_;
		};
	}

	class monotonic_buffer_resource : public memory_resource
	{

	public:

		using size_type = std::size_t;

	private:

		static constexpr size_type MaxBufferSize = 1024 * 1024; //1MB

	public:

		explicit monotonic_buffer_resource(memory_resource_ptr upstream)
			:poolManager_(upstream)
		{
		}

		monotonic_buffer_resource(size_type initialSize, memory_resource_ptr upstream)
			:poolManager_{ upstream }
		{
			nextBufferSize_ = Internal::GetFinalSize(initialSize);
		}

		monotonic_buffer_resource(void* buffer, size_type bufferSize, memory_resource_ptr upstream)
			:bufferManager_{buffer,bufferSize}
		{
			auto nextSize = Internal::GetFinalSize(bufferSize) << 1;

			if (nextSize > MaxBufferSize) nextSize = MaxBufferSize;

			nextBufferSize_ = nextSize;
		}

		DISABLECOPY(monotonic_buffer_resource)

		~monotonic_buffer_resource() = default;

	protected:

		void* do_allocate(size_type bytes, size_type alignment) override
		{
			auto buffer = bufferManager_.Allocate(bytes, alignment);

			if (buffer != nullptr) return buffer;

			const auto size = Internal::GetFinalSize(bytes, alignment);

			if (nextBufferSize_ < size) nextBufferSize_ = size;

			buffer = poolManager_.Allocate(nextBufferSize_,alignof(std::max_align_t));

			bufferManager_.ReplaceBuffer(buffer, nextBufferSize_);

			nextBufferSize_ <<= 1;

			if (nextBufferSize_ > MaxBufferSize) nextBufferSize_ = MaxBufferSize;

			return bufferManager_.Allocate(bytes, alignment);
		}

		void do_deallocate(void* , size_type , size_type ) override
		{
			//no-op
		}

		bool do_is_equal(const memory_resource& other) const noexcept override
		{
			return this == dynamic_cast<const monotonic_buffer_resource*>(&other);
		}

	private:
		size_type nextBufferSize_;
		Internal::BufferManager bufferManager_;
		Internal::SimplePoolManager poolManager_;
	};
	
	
}


inline void *operator new(std::size_t sz, Yupei::memory_resource_ptr mrp)
{
	return mrp->allocate(sz);
}

inline void operator delete(void *p, std::size_t sz, Yupei::memory_resource_ptr mrp)
{
	mrp->deallocate(p, sz);
}


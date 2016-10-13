#include "MemoryResource.hpp"
#include "../ConstructDestruct.hpp"
#include "../Mutex.hpp"
#include <new> //for ::operator new/delete
#include <cstdint>

namespace Yupei
{
	memory_resource::~memory_resource()
	{
	}

	void* memory_resource::allocate(size_type bytes, size_type alignment)
	{
		return do_allocate(bytes, alignment);
	}

	void memory_resource::deallocate(void * p, size_type bytes, size_type alignment) noexcept
	{
		do_deallocate(p, bytes, alignment);
	}

	bool memory_resource::is_equal(const memory_resource & other) const noexcept
	{
		return do_is_equal(other);
	}

	class NewDeleteResource : public memory_resource
	{
	public:		
		void* do_allocate(size_type bytes, size_type alignment) override
		{
			return ::operator new(Internal::GetFinalSize(bytes, alignment));
		}

		void do_deallocate(void* p, size_type bytes, size_type alignment) noexcept override
		{
			return ::operator delete(p, Internal::GetFinalSize(bytes, alignment));
		}

		bool do_is_equal(const memory_resource& other) const noexcept override
		{
			return this == &other;
		}
		
	};

	class NullMemoryResource : public memory_resource
	{
	public:
		[[noreturn]]
		void* do_allocate(size_type, size_type) override
		{
			throw std::bad_alloc();
		}

		void do_deallocate(void*, size_type, size_type) noexcept override
		{
			//no-op
		}

		bool do_is_equal(const memory_resource& other) const noexcept override
		{
			return this == &other;
		}
	};

	static NewDeleteResource newDeleteResource;
	static NullMemoryResource nullMemoryResource;
	static mutex memoryResourceLock;
	
	memory_resource* new_delete_resource() noexcept
	{
		return &newDeleteResource;
	}

	memory_resource* null_memory_resource() noexcept
	{
		return &nullMemoryResource;
	}

	static memory_resource* defaultMemoryResource = new_delete_resource();

	memory_resource* get_default_resource() noexcept
	{
		return defaultMemoryResource;
	}

	memory_resource* set_default_resource(memory_resource* r) noexcept
	{
		auto temp = defaultMemoryResource;
		lock_guard<mutex> guard{ memoryResourceLock };
		if (r == nullptr) defaultMemoryResource = new_delete_resource();
		else defaultMemoryResource = r;
		return temp;
	}

	namespace Internal
	{
		void* BufferManager::Allocate(size_type size, size_type alignment)
		{
			const auto offset = GetFinalOffset(static_cast<const void*>(buffer_ + cursor_), alignment);
            if (cursor_ + offset + size > bufferSize_) return {};
			void* result = static_cast<void*>(buffer_ + cursor_ + offset);
			cursor_ += (size + offset);
			return result;
		}

		auto BufferManager::ReplaceBuffer(void* newBuffer, size_type newBufferSize) noexcept -> ByteType*
		{
			auto oldBuffer = buffer_;
			buffer_ = static_cast<ByteType*>(newBuffer);
			bufferSize_ = newBufferSize;
			cursor_ = {};

			return oldBuffer;
		}

		void* SimplePoolManager::Allocate(size_type size, size_type alignment)
		{
            if (size == 0) return {};

			const auto totalSize = size + sizeof(Block);

			auto block = static_cast<Block*>(upstream_->allocate(totalSize,alignment));

			block->nextBlock_ = headBlock_;
			block->blockSize_ = totalSize;
			headBlock_ = block;

			return static_cast<void*>(block + 1);

		}

		void SimplePoolManager::Release() noexcept
		{
			while (headBlock_)
			{
				auto nextBlock = headBlock_->nextBlock_;
				upstream_->deallocate(headBlock_,headBlock_->blockSize_);
				headBlock_ = nextBlock;
			}
		}
	}

	void* monotonic_buffer_resource::do_allocate(size_type bytes, size_type alignment)
	{
		auto buffer = bufferManager_.Allocate(bytes, alignment);
		if (buffer != nullptr) return buffer;
		const auto size = Internal::GetFinalSize(bytes, alignment);
		if (nextBufferSize_ < size) nextBufferSize_ = size;
		buffer = poolManager_.Allocate(nextBufferSize_, alignof(std::max_align_t));
		bufferManager_.ReplaceBuffer(buffer, nextBufferSize_);
		nextBufferSize_ <<= 1;
		if (nextBufferSize_ > MaxBufferSize) nextBufferSize_ = MaxBufferSize;
		return bufferManager_.Allocate(bytes, alignment);
	}

	namespace Internal
	{
		void Pool::ReAllocateChunk()
		{
			chunkBegin_ = static_cast<ByteType*>(poolManager_.Allocate(currentChunkSize_ * realBlockSize_));
			chunkEnd_ = chunkBegin_ + currentChunkSize_ * realBlockSize_;

			currentChunkSize_ <<= 1;

			if (currentChunkSize_ > maxBlocks_) currentChunkSize_ = maxBlocks_;
		}

		//FIXME
		void* Pool::Allocate(size_type/* bytes*/, size_type/* alignment*/)
		{
			if (chunkBegin_ == chunkEnd_)
			{
				if (freeList_)
				{
					auto p = freeList_;
					freeList_ = p->nextBlock_;
					p->nextBlock_ = {};
					return static_cast<void*>(p);
				}
				ReAllocateChunk();
			}

			auto p = chunkBegin_;
			chunkBegin_ += realBlockSize_;
			return static_cast<void*>(p);
		}

		void Pool::Deallocate(void* address) noexcept
		{
			auto p = static_cast<Link*>(address);
			p->nextBlock_ = freeList_;
			freeList_ = p;
		}

		void Pool::Release() noexcept
		{
			freeList_ = {};
			chunkBegin_ = {};
			chunkEnd_ = {};
			currentChunkSize_ = {};
			poolManager_.Release();
		}
	}

	unsynchronized_pool_resource::unsynchronized_pool_resource(const pool_options& opts, memory_resource_ptr upstream)
		:poolManager_{upstream}
	{
		auto required = opts.largest_required_pool_block;
		if (required == 0 || required > maxPoolSize)
			required = maxPoolSize;
		required = Internal::GetFinalSize(required);
		poolsCount_ = required >> 3;
		maxBlockSize_ = required;

		pools_ = static_cast<Internal::Pool*>(::operator new(sizeof(Internal::Pool) * poolsCount_));

		for (size_type i{};i < poolsCount_;++i)
            Yupei::construct(pools_ + i, (i + 1) << 3, opts.max_blocks_per_chunk);
	}

	void* unsynchronized_pool_resource::do_allocate(size_type bytes, size_type alignment)
	{
		const auto finalSize = Internal::GetFinalSize(bytes, alignment);
		if (finalSize > maxBlockSize_)
		{
			return poolManager_.Allocate(bytes, alignment);
		}
		const auto poolIndex = FindPool(bytes);
		return pools_[poolIndex].Allocate(bytes, alignment);
	}

	void unsynchronized_pool_resource::do_deallocate(void * p, size_type bytes, size_type alignment) noexcept
	{
		auto finalSize = Internal::GetFinalSize(bytes, alignment);
		if (finalSize > maxBlockSize_) return poolManager_.Deallocate(p);
		const auto poolIndex = FindPool(bytes);
		pools_[poolIndex].Deallocate(p);
	}

	bool unsynchronized_pool_resource::do_is_equal(const memory_resource& other) const noexcept
	{
		return this == dynamic_cast<const unsynchronized_pool_resource*>(&other);
	}
}



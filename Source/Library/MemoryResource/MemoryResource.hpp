#pragma once

#include "..\Extensions.hpp"
#include <cstddef>
#include <cstdint>

namespace Yupei
{
    class memory_resource
    {
        static constexpr std::size_t max_align = alignof(std::max_align_t);

    public:
        using size_type = std::size_t;
        void* allocate(size_type bytes, size_type alignment = max_align);
        void deallocate(void* p, size_type bytes, size_type alignment = max_align) noexcept;
        bool is_equal(const memory_resource& other) const noexcept;
        virtual ~memory_resource();

    protected:
        virtual void* do_allocate(size_type bytes, size_type alignment) = 0;
        virtual void do_deallocate(void* p, size_type bytes, size_type alignment) noexcept = 0;
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
            : resource_{get_default_resource()}
        {}

        explicit memory_resource_ptr(std::nullptr_t) noexcept
            : resource_{get_default_resource()}
        {}

        explicit memory_resource_ptr(memory_resource* p) noexcept
            : resource_{p ? p : get_default_resource()}
        {}

        memory_resource_ptr(const memory_resource_ptr&) noexcept = default;

        ~memory_resource_ptr() = default;

        memory_resource& operator =(const memory_resource_ptr&) = delete;

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

    inline bool operator==(memory_resource_ptr a, memory_resource_ptr b) noexcept
    {
        return a.get() == b.get();
    }

    inline bool operator!=(memory_resource_ptr a, memory_resource_ptr b) noexcept
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

        explicit polymorphic_allocator(memory_resource_ptr r)
            :resource_{r}
        {
        }

        polymorphic_allocator(const polymorphic_allocator& other) noexcept
            :resource_{other.resource_}
        {}

        polymorphic_allocator& operator=(const polymorphic_allocator& other) noexcept
        {
            resource_.reset(other.resource());
            return *this;
        }

        value_type* allocate(size_type n)
        {
            return static_cast<value_type*>(resource_->allocate(n * sizeof(value_type), alignof(value_type)));
        }

        void deallocate(value_type* p, size_type n) noexcept
        {
            resource_->deallocate(p, n * sizeof(value_type), alignof(value_type));
        }

        template <typename U>
        polymorphic_allocator(const polymorphic_allocator<U>& other) noexcept
            :resource_{other.resource()}
        {}

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
        constexpr inline memory_resource::size_type GetFinalSize(memory_resource::size_type bytes, memory_resource::size_type alignment = alignof(std::max_align_t)) noexcept
        {
            return (bytes + alignment - 1) & ~(alignment - 1);
        }

        inline std::size_t GetFinalOffset(const void* addr, std::size_t alignment) noexcept
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

            BufferManager(void* buffer, size_type bufferSize) noexcept
                :buffer_{static_cast<ByteType*>(buffer)},
                bufferSize_{bufferSize},
                cursor_{}
            {
            }

            constexpr BufferManager() noexcept
                :buffer_{}, bufferSize_{}, cursor_{}
            {}

            void* Allocate(size_type size, size_type alignment);

            ByteType* ReplaceBuffer(void* newBuffer, size_type newBufferSize) noexcept;

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

            void Deallocate(void* ptr) noexcept
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
                : headBlock_{}
            {}

            SimplePoolManager(memory_resource_ptr upstream)
                :headBlock_{}, upstream_{upstream}
            {}

            void* Allocate(size_type size, size_type alignment = alignof(std::max_align_t));

            void Deallocate(void* ptr) noexcept
            {
                //no-op
            }

            memory_resource_ptr GetUpstream() const noexcept
            {
                return upstream_;
            }

            void Release() noexcept;

            ~SimplePoolManager() = default;

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
            :poolManager_{upstream}
        {
        }

        monotonic_buffer_resource(size_type initialSize, memory_resource_ptr upstream)
            :poolManager_{upstream}
        {
            nextBufferSize_ = Internal::GetFinalSize(initialSize);
        }

        monotonic_buffer_resource(void* buffer, size_type bufferSize, memory_resource_ptr upstream)
            :bufferManager_{buffer,bufferSize},
            poolManager_{upstream}
        {
            auto nextSize = Internal::GetFinalSize(bufferSize) << 1;
            if (nextSize > MaxBufferSize) nextSize = MaxBufferSize;
            nextBufferSize_ = nextSize;
        }

        void release() noexcept
        {
            nextBufferSize_ = {};
            bufferManager_.Release();
            poolManager_.Release();
        }

        DISABLECOPY(monotonic_buffer_resource)

            ~monotonic_buffer_resource()
        {
            release();
        }

    protected:

        void* do_allocate(size_type bytes, size_type alignment) override;

        void do_deallocate(void*, size_type, size_type) noexcept override
        {
            //no-op
        }

        bool do_is_equal(const memory_resource& other) const noexcept override
        {
            return this == dynamic_cast<const monotonic_buffer_resource*>(&other);
        }

    private:
        size_type nextBufferSize_ = {};
        Internal::BufferManager bufferManager_;
        Internal::SimplePoolManager poolManager_;
    };

    namespace Internal
    {
        class Pool
        {
        public:
            using size_type = std::size_t;

            const size_type blockSize_;
            const size_type realBlockSize_;
            const size_type maxBlocks_;

            size_type currentChunkSize_;

            struct alignas(std::max_align_t) Link
            {
                Link* nextBlock_;
            };

            SimplePoolManager poolManager_;
            Link* freeList_ = {};

        private:
            using ByteType = unsigned char;

            static constexpr size_type initialBlocksCount = 1;

            static constexpr size_type maxBlocksCount = 64;

            void ReAllocateChunk();

        public:
            ByteType* chunkBegin_ = {};
            ByteType* chunkEnd_ = {};

        public:
            Pool(memory_resource_ptr upstream, size_type blockSize, size_type maxBlocks)
                :blockSize_{blockSize},
                realBlockSize_{GetFinalSize(blockSize)},
                maxBlocks_{maxBlocks == 0 ? maxBlocksCount : maxBlocks},
                currentChunkSize_{initialBlocksCount},
                poolManager_{upstream}
            {
            }

            Pool(size_type blockSize, size_type maxBlocks)
                :Pool{{},blockSize,maxBlocks}
            {
            }

            void* Allocate(size_type bytes, size_type alignment = alignof(std::max_align_t));

            void Deallocate(void* address) noexcept;

            void Release() noexcept;

            DISABLECOPY(Pool)

                size_type GetBlockSize() const noexcept
            {
                return blockSize_;
            }

        };
    }

    struct pool_options
    {
        std::size_t max_blocks_per_chunk = 0;
        std::size_t largest_required_pool_block = 0;
    };

    class unsynchronized_pool_resource : public memory_resource
    {
    public:
        unsynchronized_pool_resource(const pool_options& opts, memory_resource_ptr upstream);

        unsynchronized_pool_resource()
            :unsynchronized_pool_resource{pool_options(), {}}
        {}

        unsynchronized_pool_resource(const pool_options& opts)
            :unsynchronized_pool_resource{opts, {}}
        {}

        ~unsynchronized_pool_resource()
        {
            release();
        }

        DISABLECOPY(unsynchronized_pool_resource)

            void release() noexcept
        {
            for (size_type i{};i < poolsCount_;++i)
                pools_[i].Release();
            ::operator delete(pools_);
            pools_ = {};
            poolManager_.Release();
        }

        memory_resource_ptr upstream_resource() const noexcept
        {
            return poolManager_.GetUpstream();
        }

        //pool_options options() const;

    protected:
        void* do_allocate(size_type bytes, size_type alignment) override;

        void do_deallocate(void* p, size_type bytes, size_type alignment) noexcept override;

        bool do_is_equal(const memory_resource& other) const noexcept override;

    private:
        Internal::Pool* pools_;
        size_type poolsCount_;
        size_type maxBlockSize_;
        Internal::SimplePoolManager poolManager_;

        size_type FindPool(size_type bytes) const noexcept
        {
            constexpr auto maxAlign = alignof(std::max_align_t);
            return (bytes + maxAlign - 1) / maxAlign - 1;
        }

    private:
        static constexpr size_type maxPoolSize = 256;
        static constexpr size_type initialPoolSize = alignof(std::max_align_t);
    };
}


inline void *operator new(std::size_t sz, Yupei::memory_resource_ptr mrp)
{
    return mrp->allocate(sz);
}

inline void operator delete(void *p, std::size_t sz, Yupei::memory_resource_ptr mrp) noexcept
{
    mrp->deallocate(p, sz);
}


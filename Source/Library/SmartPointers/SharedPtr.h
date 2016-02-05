#pragma once

#include "..\type_traits.h"
#include "..\MemoryInternal.h"
#include "..\__swap.h"
#include "..\compress_pair.h"
#include "default_delete.h"
#include "..\compare_funtor.h"
#include "unique_ptr.h"
#include "..\atomic.h"

#include <exception> //for std::exception
#include <cassert>


namespace Yupei
{
	//http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3920.html

	class bad_weak_ptr
		:public std::exception
	{
	public:
		bad_weak_ptr() noexcept {}

		const char* what() const noexcept override
		{
			return "bad_weak_ptr";
		}
	};

	template<typename ObjectT>
	class weak_ptr;

	class RefCountBase
	{
	protected:

		virtual void OnZeroWeak() noexcept = 0;

		virtual void OnZeroShared() noexcept = 0;

		virtual ~RefCountBase() = default;

	public:
		RefCountBase() noexcept
			:refCount_{1},
			weakCount_{1}
		{
		}

		bool TryAddRef() noexcept
		{
			auto oldRef = refCount_.load(memory_order::memory_order_relaxed);
			for (;;)
			{
				if (oldRef == 0) return false;
				if (atomic_compare_exchange_strong_explicit(
					static_cast<volatile atomic<std::size_t>*>(&refCount_), 
					&oldRef, 
					oldRef + 1, 
					memory_order::memory_order_release,
					memory_order::memory_order_relaxed))
					return true;
			}
		}

		void AddRef() noexcept
		{
			++refCount_;
		}

		void DecRef() noexcept
		{
			--refCount_;
			if (refCount_.load(memory_order::memory_order_relaxed) == 0)
			{
				OnZeroShared();
				DecWeak();
			}
		}

		void AddWeak() noexcept
		{
			++weakCount_;
		}

		void DecWeak() noexcept
		{
			--weakCount_;
			if (refCount_.load(memory_order::memory_order_relaxed) == 0)
				OnZeroWeak();
		}

		std::size_t GetRefCount() const noexcept
		{
			return refCount_.load(memory_order::memory_order_relaxed);
		}

	private:
		atomic<std::size_t> refCount_;
		atomic<std::size_t> weakCount_;
	};
	

	template<typename ObjectT>
	class RawRefCount :
		public RefCountBase
	{
	public:
		RawRefCount(ObjectT* ptr)
			:ptr_(ptr) {}

	protected:		
		void OnZeroShared() noexcept override
		{
			delete ptr_;
		}
		void OnZeroWeak() noexcept override
		{
			delete this;
		}
	private:
		ObjectT* ptr_;
	};

	template<typename ObjectT>
	class RawRefCount<ObjectT[]> :
		public RefCountBase
	{
	public:
		RawRefCount(ObjectT* ptr)
			:ptr_(ptr) {}

	protected:
		void OnZeroShared() noexcept override
		{
			delete[] ptr_;
		}
		void OnZeroWeak() noexcept override
		{
			delete this;
		}
	private:
		ObjectT* ptr_;
	};

	template<typename ObjectT,typename DeleterT>
	class RefCountDeleter :
		public RefCountBase
	{
	public:
		RefCountDeleter(ObjectT* ptr, DeleterT d)
			:data_{ d, ptr } {}

	protected:
		void OnZeroShared() noexcept override
		{
			Yupei::invoke(data_.first(), data_.second());
		}
		void OnZeroWeak() noexcept override
		{
			delete this;
		}
	private:
		compress_pair<DeleterT, ObjectT*> data_;
	};
	
	//We cannot specify deleter in make_shared
	template<typename ObjectT>
	class RefCountAllocateOnce : public RefCountBase
	{
	public:
		template<typename... ParamsT>
		RefCountAllocateOnce(ParamsT&&... params)
			:object_(Yupei::forward<ParamsT>(params)...)
		{
		}

		ObjectT* GetObjectPtr() noexcept
		{
			return &object_;
		}

	protected:

		void OnZeroShared() noexcept override
		{
			Yupei::destroy(GetObjectPtr());
		}

		void OnZeroWeak() noexcept override
		{
			::operator delete(this);
		}
		
	private:

		ObjectT object_;
	};

	template<typename ObjectT>
	class RefCountAllocateOnce<ObjectT[]> : public RefCountBase
	{
	public:
		RefCountAllocateOnce(ObjectT* ptr,std::size_t size)
			: ptr_(ptr),
			size_(size)
		{
		}
	protected:
		void OnZeroShared() noexcept override
		{
			for (std::size_t i = 0;i < size_;++i)
			{
				ptr_[i].~ObjectT();
			}
		}
		void OnZeroWeak() noexcept override
		{
			::operator delete(this);
		}

	private:
		ObjectT* ptr_;
		std::size_t size_;
	};

	template<typename ObjectT,std::size_t N>
	class RefCountAllocateOnce<ObjectT[N]> : public RefCountBase
	{
	public:
		RefCountAllocateOnce(ObjectT* ptr) noexcept
			:ptr_(ptr)
		{
		}
	protected:
		void OnZeroShared() noexcept override
		{
			for (std::size_t i = 0;i < N;++i)
			{
				ptr_[i].~ObjectT();
			}
		}
		void OnZeroWeak() noexcept override
		{
			::operator delete(this);
		}

	private:
		ObjectT* ptr_;
	};


	namespace Internal
	{
		//modified from Boost, workaround for VS2015.

		template<typename Y, typename T>
		struct SpConvertible
		{
			using yes = char(&)[1];
			using no = char(&)[2];

			static yes f(T*);
			static no f(...);

			static constexpr bool value = sizeof(f(static_cast<Y*>(0))) == sizeof(yes);
		};

		template<typename Y, typename T>
		struct SpConvertible<Y, T[]>
		{
			static constexpr bool value = false;
		};

		template<typename Y, typename T>
		struct SpConvertible<Y[], T[]>
		{
			static constexpr bool value = SpConvertible<Y[1], T[1]>::value;
		};

		template<typename Y,std::size_t N,  typename T>
		struct SpConvertible<Y[N], T[]>
		{
			static constexpr bool value = SpConvertible<Y[1], T[1]>::value;
		};
	}

	template<typename ObjectT>
	class enable_shared_from_this;

	template<typename ObjectT>
	class PtrBase
	{
	public:
		
		template<typename RhsObjectT>
		friend class PtrBase;
		template<typename T>
		friend class weak_ptr;

		using element_type = remove_extent_t<ObjectT>;

		constexpr PtrBase() noexcept
			:refCount_{},
			rawPtr_{}
		{
		}

		PtrBase(PtrBase&& rhs) noexcept
			:refCount_{rhs.refCount_},
			rawPtr_{rhs.rawPtr_}
		{
			rhs.refCount_ = {};
			rhs.rawPtr_ = {};
		}

		template<typename RhsObjectT>
		PtrBase(PtrBase<RhsObjectT>&& rhs)
			:refCount_{ rhs.refCount_ },
			rawPtr_{ rhs.rawPtr_ }
		{
			rhs.refCount_ = {};
			rhs.rawPtr_ = {};
		}

		std::size_t use_count() const noexcept
		{
			if (refCount_) return refCount_->GetRefCount();
			return{};
		}


	protected:
		void Reset(element_type* p, RefCountBase* refCount, bool isThrow)
		{
			if (refCount && refCount->TryAddRef())
			{
				DecRef();
				refCount_ = refCount;
			}
			else if(isThrow)
				throw bad_weak_ptr();
		}


		void Swap(PtrBase& rhs) noexcept
		{
			using Yupei::swap;
			swap(refCount_, rhs.refCount_);
			swap(rawPtr_, rhs.rawPtr_);
		}

		//postcondition: refCount_ == nullptr
		void Reset(element_type* p, RefCountBase* refCount)
		{
			if(refCount)
				refCount->AddRef();
			refCount_ = refCount;
			rawPtr_ = p;
		}

		void DecRef() noexcept
		{
			if (refCount_)
				refCount_->DecRef();
		}

		void AddRef() noexcept
		{
			if (refCount_)
				refCount_->AddRef();
		}

		template<typename RhsObjectT>
		void ResetW(const PtrBase<RhsObjectT>& otherPtr)
		{
			ResetW(otherPtr.rawPtr_, otherPtr.refCount_);
		}

		//这是为了enable_shared_from_this<T>:
		//其内置的weak_ptr<T>为非const，当类为const时，不能将const T*赋值给T*
		//故使用const_cast去掉const。
		template<typename ObjectRhsT>
		void ResetW(const ObjectRhsT* p, RefCountBase* ref_count)
		{
			ResetW(const_cast<ObjectRhsT*>(p), ref_count);
		}

		template<typename ObjectRhsT>
		void ResetW(ObjectRhsT* p, RefCountBase* _ref_count)
		{
			if (this->refCount_ != nullptr)
				refCount_->DecWeak();	
			if (_ref_count)
				_ref_count->AddWeak();
			this->refCount_ = _ref_count;
			this->rawPtr_ = p;			
		}
	
	protected:

		RefCountBase* refCount_;
		element_type* rawPtr_;
	};

	namespace Internal
	{

		template<typename T>
		void ValueInitializeImp(T* obj, std::size_t count,true_type)
		{
			memset(static_cast<void*>(obj), 0, count * sizeof(T));
		}

		template<typename T>
		void ValueInitializeImp(T* obj, std::size_t count,false_type)
		{
			for (std::size_t i = 0;i < count;++i)
			{
				void* p = static_cast<void*>(obj + i);
				::new (p) T();
			}
		}

		template<typename T>
		void ValueInitialize(T* obj, std::size_t count)
		{
			return Internal::ValueInitializeImp(obj, count, is_pod<T>());
		}
	}

	template<typename ObjectT>
	class shared_ptr : public PtrBase<ObjectT>
	{
	public:
		template<typename T>
		friend class shared_ptr;

		using BaseType = PtrBase<ObjectT>;
		using ThisType = shared_ptr<ObjectT>;
		using element_type = typename BaseType::element_type;

		constexpr shared_ptr() noexcept = default;


		//Y shall be a complete type. 
		//The expression delete[] p, when T is an array type, or delete p, 
		//when T is not an array type, shall be well-formed, shall have well defined behavior,
		//and shall not throw exceptions. When T is U[N], Y(*)[N] shall be convertible to T*; 
		//when T is U[], Y(*)[] shall be convertible to T*; otherwise, Y* shall be convertible to T*.
		template<typename Y>
		explicit shared_ptr(Y* p)
		{
			this->rawPtr_ = p;
			auto ref = make_unique<RawRefCount<ObjectT>>(p);
			this->refCount_ = ref.release();
		}

		template<typename RhsObjectT,typename DeleterT>
		shared_ptr(RhsObjectT* p, DeleterT deleter)
		{
			this->rawPtr_ = p;
			auto ref = make_unique<RefCountDeleter<ObjectT,DeleterT>>(p,deleter);
			this->refCount_ = ref.release();
		}

		template<typename DeleterT>
		shared_ptr(std::nullptr_t p, DeleterT deleter)
			:shared_ptr(nullptr,deleter)
		{
		
		}

		template<typename RhsObjectT>
		shared_ptr(const shared_ptr<RhsObjectT>& r, element_type* p)
		{
			this->Reset(p, r.refCount_);
		}

		shared_ptr(const shared_ptr& r) noexcept
		{
			this->Reset(r.rawPtr_, r.refCount_);
		}

		template<typename RhsObjectT,
			typename = enable_if_t<Internal::SpConvertible<RhsObjectT, ObjectT>::value>>
		shared_ptr(const shared_ptr<RhsObjectT>& r) noexcept
		{
			this->Reset(r.rawPtr_, r.refCount_);
		}

		shared_ptr(shared_ptr&& r) noexcept
			:BaseType(Yupei::move(r))
		{
		}

		template<typename RhsObjectT,
			typename = enable_if_t<Internal::SpConvertible<RhsObjectT, ObjectT>::value>>
		shared_ptr(shared_ptr<RhsObjectT>&& r) noexcept
			:BaseType(Yupei::move(r))
		{			
		}

		
		template<typename RhsObjectT,
			typename = enable_if_t<Internal::SpConvertible<RhsObjectT, ObjectT>::value>>
		explicit shared_ptr(const weak_ptr<RhsObjectT>& r)
		{
			this->Reset(r.rawPtr_,r.refCount_, true);
		}
		
		constexpr shared_ptr(std::nullptr_t) noexcept
			: shared_ptr()
		{
		}

		~shared_ptr()
		{
			if (this->refCount_)
				this->refCount_->DecRef();
		}

		shared_ptr& operator=(const shared_ptr& r) noexcept
		{
			shared_ptr(r).swap(*this);
			return *this;
		}

		template<typename RhsObjectT>
		shared_ptr& operator=(const shared_ptr<RhsObjectT>& rhs) noexcept
		{
			shared_ptr(rhs).swap(*this);
			return *this;
		}
		
		shared_ptr& operator=(shared_ptr&& r) noexcept
		{
			shared_ptr(Yupei::move(r)).swap(*this);
			return *this;
		}
		
		template<typename RhsObjectT>
		shared_ptr& operator=(shared_ptr<RhsObjectT>&& r) noexcept
		{
			shared_ptr(Yupei::move(r)).swap(*this);
			return *this;
		}

		void swap(shared_ptr& rhs)
		{
			this->Swap(rhs);
		}

		void reset() noexcept
		{
			shared_ptr().swap(*this);
		}

		template<typename Y>
		void reset(Y* p)
		{
			shared_ptr(p).swap(*this);
		}

		template<typename Y,typename D>
		void reset(Y* p, D d)
		{
			shared_ptr(p,d).swap(*this);
		}

		
		element_type* get() const noexcept
		{
			return this->rawPtr_;
		}

		template<typename Dummy = ObjectT,
			typename = enable_if_t<!is_array<Dummy>::value && !is_void<remove_cv_t<Dummy>>::value>>
		element_type& operator*() const noexcept
		{
			assert(get() != 0);
			return *get();
		}

		template<typename Dummy = ObjectT, 
			typename = enable_if_t<!is_array<Dummy>::value && !is_void<remove_cv_t<Dummy>>::value>>
		element_type* operator->() const noexcept
		{
			assert(get() != 0);
			return get();
		}

		template<typename Dummy = ObjectT, typename = enable_if_t<is_array<Dummy>::value>>
		element_type& operator[](std::ptrdiff_t i) const noexcept
		{
			return get()[i];
		}

		bool unique() const noexcept
		{
			return this->use_count() == 1;
		}

		explicit operator bool() const noexcept
		{
			return get() != 0;
		}		
		
		template<
			typename Dummy = ObjectT,
			typename = enable_if_t<!is_array<Dummy>::value>,
			typename... Args>
		static shared_ptr make_shared(Args&&... args)
		{
			auto holder = Yupei::make_unique<RefCountAllocateOnce<element_type>>(Yupei::forward<Args>(args)...);
			shared_ptr ptr{};
			ptr.rawPtr_ = holder->GetObjectPtr();
			ptr.refCount_ = holder.release();
			ptr.DoEnable(ptr.get());
			return ptr;
		}

		template<typename Dummy = ObjectT, 
			typename = enable_if_t<is_array<Dummy>::value && extent<Dummy>::value == 0>>
		static shared_ptr
			make_shared(std::size_t n)
		{
			RefCountBase* ref;
			element_type* ele;
			AllocateArray(n, ref, ele);			
			new (ref) RefCountAllocateOnce<ObjectT>(ele,n);
			shared_ptr<ObjectT> ret;
			ret.refCount_ = ref;
			ret.rawPtr_ = ele;
			return ret;
		}

		template<typename Dummy = ObjectT,
			typename = enable_if_t<is_array<Dummy>::value && extent<Dummy>::value != 0>>
		static shared_ptr
			make_shared()
		{
			RefCountBase* ref;
			element_type *ele;
			AllocateArray(extent<ObjectT>::value, ref, ele);
			new (ref) RefCountAllocateOnce<ObjectT>(ele);
			shared_ptr<ObjectT> ret;
			ret.refCount_ = ref;
			ret.rawPtr_ = ele;
			return ret;
		}

		private:
			void DoEnable(const volatile void*) const
			{
			}

			template<typename ObjectRhsT>
			void DoEnable(const enable_shared_from_this<ObjectRhsT>* p) const;

			
			static void AllocateArray(std::size_t n, RefCountBase*& ref,element_type*& ele)
			{
				constexpr auto alignSize = alignof(element_type);
				auto n1 = sizeof(RefCountAllocateOnce<ObjectT>);
				auto n2 = sizeof(element_type) * n;
				auto n3 = n2 + alignSize;
				auto p1 = ::operator new(n1 + n3);
				void* p2 = static_cast<char*>(p1) + n1;
				ref = static_cast<RefCountBase*>(p1);
				ele = static_cast<element_type*>(p2);
				Internal::ValueInitialize(ele, n);
			}
	};

	template<typename ObjectT,typename... Args>
	shared_ptr<ObjectT> make_shared(Args&&... args)
	{
		return shared_ptr<ObjectT>::make_shared(Yupei::forward<Args>(args)...);
	}

	template<typename ObjectT,typename ObjectRhsT>
	bool operator==(const shared_ptr<ObjectT>& lhs, const shared_ptr<ObjectRhsT>& rhs) noexcept
	{
		return lhs.get() == rhs.get();
	}

	template<typename ObjectT, typename ObjectRhsT>
	bool operator!=(const shared_ptr<ObjectT>& lhs,const shared_ptr<ObjectRhsT>& rhs) noexcept
	{
		return lhs.get() != rhs.get();
	}

	template<typename ObjectT, typename ObjectRhsT>
	bool operator < (const shared_ptr<ObjectT>& lhs,const shared_ptr<ObjectRhsT>& rhs) noexcept
	{
		return Yupei::less<>()(lhs.get(), rhs.get());
	}

	template<typename ObjectT, typename ObjectRhsT>
	bool operator > (const shared_ptr<ObjectT>& lhs,const shared_ptr<ObjectRhsT>& rhs) noexcept
	{
		return rhs < lhs;
	}

	template<typename ObjectT, typename ObjectRhsT>
	bool operator <= (const shared_ptr<ObjectT>& lhs,const shared_ptr<ObjectRhsT>& rhs) noexcept
	{
		return !(rhs < lhs);
	}

	template<typename ObjectT, typename ObjectRhsT>
	bool operator >= (const shared_ptr<ObjectT>& lhs,const shared_ptr<ObjectRhsT>& rhs) noexcept
	{
		return !(lhs < rhs);
	}

	template<typename ObjectT, typename ObjectRhsT>
	bool operator == (const shared_ptr<ObjectT>& lhs,std::nullptr_t) noexcept
	{
		return lhs.get() == nullptr;
	}

	template<typename ObjectT, typename ObjectRhsT>
	bool operator==(std::nullptr_t, const shared_ptr<ObjectT>& rhs) noexcept
	{
		return nullptr == rhs.get();
	}

	template<typename ObjectT, typename ObjectRhsT>
	bool operator!=(const shared_ptr<ObjectT>& lhs, std::nullptr_t) noexcept
	{
		return static_cast<bool>(lhs);
	}

	template<typename ObjectT, typename ObjectRhsT>
	bool operator!=(std::nullptr_t,const shared_ptr<ObjectT>& rhs) noexcept
	{
		return static_cast<bool>(rhs);
	}

	template<typename ObjectT, typename ObjectRhsT>
	bool operator < (const shared_ptr<ObjectT>& lhs, std::nullptr_t) noexcept
	{
		return less<>()(lhs.get(), nullptr);
	}

	template<typename ObjectT, typename ObjectRhsT>
	bool operator < (std::nullptr_t,const shared_ptr<ObjectT>& rhs) noexcept
	{
		return less<>()(nullptr, rhs.get());
	}

	template<typename ObjectT, typename ObjectRhsT>
	bool operator > (const shared_ptr<ObjectT>& lhs, std::nullptr_t) noexcept
	{
		return nullptr < lhs;
	}

	template<typename ObjectT, typename ObjectRhsT>
	bool operator > (std::nullptr_t, const shared_ptr<ObjectT>& rhs) noexcept
	{
		return rhs < nullptr;
	}

	template<typename ObjectT>
	void swap(shared_ptr<ObjectT>& lhs,shared_ptr<ObjectT>& rhs) noexcept
	{
		lhs.swap(rhs);
	}

	// 20.8.2.2.9, shared_ptr casts:
	template<typename ObjectT,typename ObjectRhsT>
	shared_ptr<ObjectT> 
		static_pointer_cast(const shared_ptr<ObjectRhsT>& r) noexcept
	{
		shared_ptr<ObjectT> ptr{ r,static_cast<typename shared_ptr<ObjectT>::element_type*>(r.get()) };
		return ptr;
	}

	template<typename ObjectT,typename ObjectRhsT>
	shared_ptr<ObjectT>
		dynamic_pointer_cast(const shared_ptr<ObjectRhsT>& r) noexcept
	{
		auto rawPtr = dynamic_cast<typename shared_ptr<ObjectT>::element_type*>(r.get());
		if(rawPtr) 
			return shared_ptr<ObjectT>{r, rawPtr};
		else 
			return shared_ptr<ObjectT>();
	}

	template<typename ObjectT, typename ObjectRhsT>
	shared_ptr<ObjectT>
		reinterpret_pointer_cast(const shared_ptr<ObjectRhsT>& r) noexcept
	{
		shared_ptr<ObjectT> ptr{ r,reinterpret_cast<typename shared_ptr<ObjectT>::element_type*>(r.get()) };
		return ptr;
	}

	template<typename ObjectT,	typename ObjectRhsT>
	auto
		const_pointer_cast(const shared_ptr<ObjectRhsT>& r) noexcept
	{
		shared_ptr<ObjectT> ptr{ r,const_cast<typename shared_ptr<ObjectT>::element_type*>(r.get()) };
		return ptr;
	}

	template<typename ObjectT>
	class weak_ptr : public PtrBase<ObjectT>
	{
	public:
		template<typename ObjectRhsT>
		friend class shared_ptr;

		using BaseType = PtrBase<ObjectT>;
		using element_type = typename BaseType::element_type;

		constexpr weak_ptr() noexcept = default;

		weak_ptr(const weak_ptr& rhs)
		{
			this->ResetW(rhs);
		}

		template<typename RhsObjectT,
			typename = enable_if_t<Internal::SpConvertible<RhsObjectT, element_type>::value>>
		weak_ptr(const shared_ptr<RhsObjectT>& r) noexcept
		{
			this->ResetW(r);
		}
		
		weak_ptr(weak_ptr&& r) noexcept
			:BaseType(Yupei::move(r))
		{
		}

		template<typename RhsObjectT,
			typename = enable_if_t<Internal::SpConvertible<RhsObjectT, element_type>::value>>
		weak_ptr(weak_ptr<RhsObjectT>&& r) noexcept
			:BaseType(Yupei::move(r))
		{
		}

		
		~weak_ptr()
		{
			if (this->refCount_)
				this->refCount_->DecWeak();
		}
		
		weak_ptr& operator=(const weak_ptr& r) noexcept
		{
			weak_ptr(r).swap(*this);
			return *this;
		}

		template<typename RhsObjectT>
		weak_ptr& operator=(const weak_ptr<RhsObjectT>& r) noexcept
		{
			this->ResetW(r);
			return *this;
		}

		template<typename RhsObjectT>
		weak_ptr& operator=(const shared_ptr<RhsObjectT>& r) noexcept
		{
			this->ResetW(r);
			return *this;
		}

		weak_ptr& operator=(weak_ptr&& r) noexcept
		{
			weak_ptr(Yupei::move(r)).swap(*this);
			return *this;
		}

		template<typename RhsObjectT>
		weak_ptr& operator=(weak_ptr<RhsObjectT>&& r) noexcept
		{
			weak_ptr(Yupei::move(r)).swap(*this);
			return *this;
		}

		void reset() noexcept
		{
			weak_ptr().swap(*this);
		}

		bool expired() const noexcept
		{
			return this->use_count() == 0;
		}

		shared_ptr<ObjectT> lock() const
		{
			shared_ptr<ObjectT> ptr{};
			ptr.Reset(this->rawPtr_,this->refCount_,false);
			return ptr;
		}

		void swap(weak_ptr<ObjectT>& rhs)
		{
			this->Swap(rhs);
		}
	};

	template<typename ObjectT>
		void swap(
			weak_ptr<ObjectT>& lhs,
			weak_ptr<ObjectT>& rhs) noexcept
	{
		lhs.swap(rhs);
	}

	template<typename ObjectT>
	class enable_shared_from_this 
	{
	protected:
		
		template<typename T>
		friend class shared_ptr;

		constexpr enable_shared_from_this() noexcept = default;

		enable_shared_from_this(const enable_shared_from_this&) = default;

		enable_shared_from_this& operator=(enable_shared_from_this const&)
		{
			return *this;
		}

		~enable_shared_from_this() = default;

	public:
		shared_ptr<ObjectT> shared_from_this()
		{
			return shared_ptr<ObjectT>(weakPtr);
		}

		shared_ptr<ObjectT> shared_from_this() const
		{
			return shared_ptr<ObjectT>(weakPtr);
		}
	private:
		mutable weak_ptr<ObjectT> weakPtr;
	};

	template<typename ObjectT>
	template<typename ObjectRhsT>
	void shared_ptr<ObjectT>::DoEnable(const enable_shared_from_this<ObjectRhsT>* p) const
	{
		if (p)
		{
			p->weakPtr.ResetW(*this);
		}
	}
}


#pragma once

#include "..\type_traits.h"
#include "..\utility_internal.h"
#include "..\__swap.h"
#include "..\compress_pair.h"
#include "default_delete.h"
#include "..\compare_funtor.h"
#include "unique_ptr.h"
#include <exception> //for std::exception
#include <cassert>


namespace Yupei
{
	class bad_weak_ptr
		:public std::exception
	{
	public:
		bad_weak_ptr() noexcept {}

		virtual const char* what() const override
		{
			return "bad_weak_ptr";
		}
	};

	template<typename ObjectT, typename Inc, typename DeleterT>
	class weak_ptr;

	//This is an unsynchronized of std::shared_ptr called unsynchronized_ptr.
	//IncRefPolicy should have following member functions:
	//void IncreaseRefCount(std::ptrdiff_t&)
	//bool DecreaseRefCount(std::ptrdiff_t& ref_count) (after the decreasing,if ref_count == 0,return true)
	//bool TryIncreaseRefCount(std::ptrdiff_t& ref_count)
	//Before we write the real ptr,we need some contracts on IncRefPolicy.
	//Maybe we can't validate the signature because of the template functions.
	//We could validate the signature by Expression SFINAE,but...
	
	template<typename Policy,typename = void>
	struct IncRefPolicyContracts : false_type
	{
		
	};

	template<typename Policy>
	struct IncRefPolicyContracts<Policy,
		void_t<decltype(&Policy::IncreaseRefCount),
		decltype(&Policy::DecreaseRefCount),
		decltype(&Policy::TryIncreaseRefCount)>> : true_type
	{

	};

	class RefCountBase
	{
	protected:
		virtual void OnZeroWeak() noexcept = 0;
		virtual void OnZeroShared() noexcept = 0;
		virtual ~RefCountBase() {}
	};


	template<typename IncRefPolicy>
	struct RefCount : public RefCountBase
	{
	public:
		//static_assert(IncRefPolicyContracts<IncRefPolicy>::value, "The policy doesn't meet the require.");
		RefCount()
			:weakCount(1),
			refCount(1)
		{

		}
		void IncRefCount()
		{
			IncRefPolicy().IncreaseRefCount(refCount);
		}
		void DecRefCount()
		{
			if (IncRefPolicy().DecreaseRefCount(refCount))
			{
				this->OnZeroShared();
				DecWeakCount();
			}
		}
		void IncWeakCount()
		{
			IncRefPolicy().IncreaseRefCount(weakCount);
		}
		void DecWeakCount()
		{
			if (IncRefPolicy().DecreaseRefCount(weakCount))
			{
				this->OnZeroWeak();
			}
		}
		std::ptrdiff_t GetWeakCount() const noexcept
		{
			return weakCount;
		}
		std::ptrdiff_t GetRefCount() const noexcept
		{
			return refCount;
		}
		bool TryIncRefCount()
		{
			return IncRefPolicy().TryIncreaseRefCount(refCount);
		}
	private:
		std::ptrdiff_t weakCount;
		std::ptrdiff_t refCount;
	};
	//IncRefPolicy should have following member functions:
	//IncreaseRefCount
	//DecreaseRefCount
	//TryIncreaseRefCount

	template<
		typename IncRefPolicy,
		typename DeleterT,
		typename ObjectT>
	class RefCountBasis :
		public RefCount<IncRefPolicy>
	{
	public:
		RefCountBasis(ObjectT* ptr)
			:ptr_(ptr) {}
	protected:
		
		void OnZeroShared() noexcept override
		{
			DeleterT()(ptr_);
		}
		void OnZeroWeak() noexcept override
		{
			delete this;
		}
	private:
		ObjectT* ptr_;
	};
	
	template<
		typename IncRefPolicy,
		typename ObjectT>
	class RefCountAllocateOnce :
		public RefCount<IncRefPolicy>
	{
	public:
		template<typename... Args>
		RefCountAllocateOnce(Args&&...)
			:object_(Yupei::forward<Args>(args)...)
		{
		}
		RefCount<IncRefPolicy>* GetRefCountPtr() noexcept
		{
			return static_cast<RefCount<IncRefPolicy>*>(this);
		}
		ObjectT* GetObjectPtr() noexcept
		{
			return &object_;
		}
	protected:
		void OnZeroShared() noexcept override
		{
			object_.~ObjectT();
		}
		void OnZeroWeak() noexcept override
		{
			::operator delete(this);
		}
		
	private:
		ObjectT object_;
	};

	struct UnsynchronizedPolicy
	{
		constexpr UnsynchronizedPolicy() noexcept = default;
		void IncreaseRefCount(std::ptrdiff_t& num)
		{
			++num;
		}
		bool DecreaseRefCount(std::ptrdiff_t& num)
		{
			if (--num == 0) return true;
			return false;
		}
		//true means success
		bool TryIncreaseRefCount(std::ptrdiff_t& num)
		{
			if (num == 0) return false;
			else ++num;
			return true;
		}
	};

	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT = default_delete<ObjectT >>
	class enable_shared_from_this;

	

	template<typename ObjectT,
		typename IncRefCountPolicy,
		typename DeleterT
		>
	class PtrBase
	{
	public:
		
		template<typename T,typename Inc,typename D>
		friend class PtrBase;
		template<typename T, typename Inc, typename D>
		friend class weak_ptr;

		using element_type = remove_extent_t<ObjectT>;
		using ref_count_type = RefCount<IncRefCountPolicy>;
		using inc_ref_policy = IncRefCountPolicy;
		using deleter_type = DeleterT;

		template<typename OtherT,typename OtherD>
		using RealBaseType = PtrBase<OtherT, inc_ref_policy, OtherD>;

		constexpr PtrBase() noexcept
			:refCountObj(),
			rawPtr()
		{

		}
		template<typename Y>
		explicit PtrBase(Y* p)
			:rawPtr(p)
		{
			auto holder = Yupei::make_unique<RefCountBasis<inc_ref_policy, DeleterT, element_type>>(rawPtr);
			refCountObj = holder.release();
		}
		PtrBase(PtrBase&& rhs)
			:refCountObj(rhs.refCountObj),
			rawPtr(rhs.rawPtr)
		{
			rhs.refCountObj = nullptr;
			rhs.rawPtr = nullptr;
		}
		template<typename OtherT,typename OtherD>
		PtrBase(RealBaseType<OtherT,OtherD>&& r) noexcept
			: refCountObj(r.refCountObj),
			rawPtr(r.rawPtr)
		{
			r.refCountObj = nullptr;
			r.rawPtr = nullptr;
		}
		std::size_t use_count() const noexcept
		{
			if (refCountObj) return refCountObj->GetRefCount();
			return{};
		}
		void swap(PtrBase& rhs)
		{
			Yupei::swap(refCountObj, rhs.refCountObj);
			Yupei::swap(rawPtr, rhs.rawPtr);
		}
	protected:
		template<typename OtherT,
			typename OtherD>
			void Reset(const RealBaseType<OtherT, OtherD>& otherPtr)
		{
			Reset(otherPtr.rawPtr, otherPtr.refCountObj);
		}
		template<typename OtherT,
			typename OtherD,
			typename OtherPtrT>
			void Reset(const RealBaseType<OtherT, OtherD>& otherPtr,OtherPtrT* p)
		{
			Reset(p, otherPtr.refCountObj);
		}
		template<typename OtherT,
			typename OtherD>
			void Reset(const RealBaseType<OtherT, OtherD>& otherPtr, bool isThrow)
		{
			Reset(otherPtr.rawPtr, otherPtr.refCountObj, isThrow);
		}
		void Reset(element_type* p, ref_count_type* _ref_count, bool isThrow)
		{
			if (_ref_count && _ref_count->TryIncRefCount())
				NakedReset(p, _ref_count);
			else if(isThrow)
				throw Yupei::bad_weak_ptr();
		}
		void NakedReset(element_type* p, ref_count_type* _ref_count)
		{
			if (this->refCountObj)
				this->refCountObj->DecRefCount();
			this->refCountObj = _ref_count;
			this->rawPtr = p;
		}
		void Reset(element_type* p, ref_count_type* _ref_count)
		{
			if (_ref_count != nullptr)
			{
				_ref_count->IncRefCount();
			}
			NakedReset(p, _ref_count);
		}
		template<typename OtherT,
			typename OtherD>
			void ResetW(const RealBaseType<OtherT, OtherD>& otherPtr)
		{
			ResetW(otherPtr.rawPtr, otherPtr.refCountObj);
		}
		//这是为了enable_shared_from_this<T>:
		//其内置的weak_ptr<T>为非const，当类为const时，不能将const T*赋值给T*
		//故使用const_cast去掉const。
		template<typename ObjectRhsT>
		void ResetW(const ObjectRhsT* p, ref_count_type* ref_count)
		{
			ResetW(const_cast<ObjectRhsT*>(p), ref_count);
		}
		template<typename ObjectRhsT>
		void ResetW(ObjectRhsT* p, ref_count_type* _ref_count)
		{
			if (this->refCountObj != nullptr)
			{
				this->refCountObj->DecWeakCount();
			}
			this->refCountObj = _ref_count;
			this->rawPtr = p;
			if (_ref_count)
				_ref_count->IncWeakCount();
		}
		void IncRef()
		{
			if (refCountObj)
				refCountObj->IncRefCount();
		}
		bool TryIncRef()
		{
			return refCountObj->TryIncRefCount();
		}
		void DecRef()
		{
			if (refCountObj)
				refCountObj->DecRefCount();
		}
		void DecWRef()
		{
			if (refCountObj)
				refCountObj->DecWeakCount();
		}
		void IncWRef()
		{
			if (refCountObj)
				refCountObj->IncWeakCount();
		}
		std::size_t UseCount() const noexcept
		{
			if (refCountObj)
				return refCountObj->GetRefCount();
			
			return{};
		}
	protected:
		RefCount<IncRefCountPolicy>* refCountObj;
		element_type* rawPtr;
	};

	template<typename ObjectT,
		typename IncRefCountPolicy,
		typename DeleterT = default_delete<ObjectT>
	>
	class shared_ptr : public PtrBase<ObjectT, IncRefCountPolicy,DeleterT>
	{
	public:
		template<typename T,typename Inc,typename D>
		friend class shared_ptr;

		using BaseType = PtrBase<ObjectT, IncRefCountPolicy, DeleterT >;
		using element_type = typename BaseType::element_type;
		using inc_ref_policy = typename BaseType::inc_ref_policy;
		using ThisType = shared_ptr<ObjectT, IncRefCountPolicy, DeleterT>;

		template<typename OtherT,
			typename OtherD>
		using OtherPtrType = shared_ptr<OtherT, inc_ref_policy, OtherD>;
		template<typename OtherT,
			typename OtherD>
		using OtherWeakPtrType = weak_ptr<OtherT, inc_ref_policy, OtherD>;

		constexpr shared_ptr() noexcept = default;

		template<typename Y,
		typename = enable_if_t<
			is_convertible<Y*,element_type*>::value>
		>
		explicit shared_ptr(Y* p)
			:PtrBase(p)
		{
			this->DoEnable(p);
		}
		template<
			typename OtherT,
			typename OtherD
		>
			shared_ptr(const OtherPtrType<OtherT,OtherD>& r, element_type* p)
		{
			this->Reset(r,p);
		}

		shared_ptr(const shared_ptr& r) noexcept
		{
			this->Reset(r);
		}
		template<typename OtherT,
			typename OtherD,
			typename = enable_if_t<
				is_convertible<OtherD, DeleterT>::value
				&& is_convertible<OtherT*, element_type*>::value >
		>
			shared_ptr(const OtherPtrType<OtherT,OtherD>& r) noexcept
		{
			this->Reset(r);
		}
		shared_ptr(shared_ptr&& r) noexcept
			:PtrBase(Yupei::move(r))
		{

		}
		template<typename OtherT,
			typename OtherD,
			typename = enable_if_t<
			is_convertible<OtherD, DeleterT>::value
			&& is_convertible<OtherT*, element_type*>::value >
		>
			shared_ptr(OtherPtrType<OtherT, OtherD>&& r) noexcept
			:BaseType(Yupei::move(r))
		{
			
		}
		constexpr shared_ptr(nullptr_t) noexcept
			: shared_ptr()
		{

		}
		template<
			typename OtherT,
			typename OtherD,
			typename = enable_if_t<
				is_convertible<OtherT*,element_type*>::value
				&& is_convertible<OtherD,DeleterT>::value>
			>
		explicit shared_ptr(const OtherWeakPtrType<OtherT,OtherD>& r)
		{
			this->Reset(r, true);
		}
		shared_ptr& operator=(const shared_ptr& r) noexcept
		{
			shared_ptr(r).swap(*this);
			return *this;
		}
		template<
			typename OtherT,
			typename OtherD,
			typename = enable_if_t<
			is_convertible<OtherT*, element_type*>::value
			&& is_assignable<DeleterT&,const OtherD&>::value>
		>
		shared_ptr& operator=(const OtherPtrType<OtherT,OtherD>& rhs) noexcept
		{
			shared_ptr(rhs).swap(*this);
			return *this;
		}
		template<
			typename OtherT,
			typename OtherD,
			typename = enable_if_t<
			is_convertible<OtherT*, element_type*>::value
			&& is_assignable<DeleterT&, OtherD&&>::value>
		>
		shared_ptr& operator=(OtherPtrType<OtherT, OtherD>&& r) noexcept
		{
			shared_ptr(Yupei::move(r)).swap(*this);
			return *this;
		}
		shared_ptr& operator=(shared_ptr&& r) noexcept
		{
			shared_ptr(Yupei::move(r)).swap(*this);
			return *this;
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
		element_type* get() const noexcept
		{
			return this->rawPtr;
		}
		element_type& operator*() const noexcept
		{
			assert(get() != 0);
			return *get();
		}
		element_type* operator->() const noexcept
		{
			assert(get() != 0);
			return get();
		}
		std::size_t use_count() const noexcept
		{
			return this->UseCount();
		}
		bool unique() const noexcept
		{
			return use_count() == 1;
		}
		explicit operator bool() const noexcept
		{
			return get() != 0;
		}
		~shared_ptr()
		{
			this->DecRef();
		}
		// 20.8.2.2.6, shared_ptr creation
		template<typename... Args>
		static ThisType make_shared(Args&&... args)
		{
			auto holder = Yupei::make_unique<RefCountAllocateOnce<inc_ref_policy, element_type>>(Yupei::forward<Args>(args)...);
			ThisType ptr{};
			auto elePtr = holder->GetObjectPtr();
			ptr.NakedReset(elePtr, holder.release());
			ptr.DoEnable(elePtr);
			return ptr;
		}
		private:
			void DoEnable(const volatile void*) const
			{

			}

			template<typename ObjectRhsT,
				typename DeleterRhsT >
			void DoEnable(const enable_shared_from_this<ObjectRhsT, inc_ref_policy, DeleterRhsT>* p) const;
	};

	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT,
		typename... Args>
		shared_ptr<ObjectT, IncRefPolicy, DeleterT> make_shared(Args&&... args)
	{
		return shared_ptr<ObjectT, IncRefPolicy, DeleterT>::make_shared(Yupei::forward<Args>(args)...);
	}

	// 20.8.2.2.7, shared_ptr comparisons:
	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT,
		typename ObjectRhsT>
		bool operator==(
			const shared_ptr<ObjectT,IncRefPolicy,DeleterT>& lhs, 
			const shared_ptr<ObjectRhsT, IncRefPolicy, DeleterT>& rhs) noexcept
	{
		return lhs.get() == rhs.get();
	}
	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT,
		typename ObjectRhsT>
		bool operator!=(
			const shared_ptr<ObjectT, IncRefPolicy, DeleterT>& lhs,
			const shared_ptr<ObjectRhsT, IncRefPolicy, DeleterT>& rhs) noexcept
	{
		return lhs.get() != rhs.get();
	}
	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT,
		typename ObjectRhsT>
		bool operator < (
			const shared_ptr<ObjectT, IncRefPolicy, DeleterT>& lhs,
			const shared_ptr<ObjectRhsT, IncRefPolicy, DeleterT>& rhs) noexcept
	{
		return Yupei::less<>()(lhs.get(), rhs.get());
	}
	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT,
		typename ObjectRhsT>
		bool operator > (
			const shared_ptr<ObjectT, IncRefPolicy, DeleterT>& lhs,
			const shared_ptr<ObjectRhsT, IncRefPolicy, DeleterT>& rhs) noexcept
	{
		return rhs < lhs;
	}
	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT,
		typename ObjectRhsT>
		bool operator <= (
			const shared_ptr<ObjectT, IncRefPolicy, DeleterT>& lhs,
			const shared_ptr<ObjectRhsT, IncRefPolicy, DeleterT>& rhs) noexcept
	{
		return !(rhs < lhs);
	}
	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT,
		typename ObjectRhsT>
		bool operator >= (
			const shared_ptr<ObjectT, IncRefPolicy, DeleterT>& lhs,
			const shared_ptr<ObjectRhsT, IncRefPolicy, DeleterT>& rhs) noexcept
	{
		return !(lhs < rhs);
	}
	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT>
		bool operator == (
			const shared_ptr<ObjectT, IncRefPolicy, DeleterT>& lhs,
			nullptr_t) noexcept
	{
		return lhs.get() == nullptr;
	}
	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT>
		bool operator==(
			nullptr_t, 
			const shared_ptr<ObjectT, IncRefPolicy, DeleterT>& rhs) noexcept
	{
		return nullptr == rhs.get();
	}
	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT>
		bool operator!=(
			const shared_ptr<ObjectT, IncRefPolicy, DeleterT>& lhs, 
			nullptr_t) noexcept
	{
		return static_cast<bool>(lhs);
	}
	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT>
		bool operator!=(
			nullptr_t,
			const shared_ptr<ObjectT, IncRefPolicy, DeleterT>& rhs) noexcept
	{
		return static_cast<bool>(rhs);
	}

	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT>
		bool operator < (
			const shared_ptr<ObjectT, IncRefPolicy, DeleterT>& lhs, 
			nullptr_t) noexcept
	{
		return Yupei::less<>()(lhs.get(), nullptr);
	}
	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT>
		bool operator < (
			nullptr_t, 
			const shared_ptr<ObjectT, IncRefPolicy, DeleterT>& rhs) noexcept
	{
		return Yupei::less<>()(nullptr, rhs.get());
	}
	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT>
		bool operator > (
			const shared_ptr<ObjectT, IncRefPolicy, DeleterT>& lhs, 
			nullptr_t) noexcept
	{
		return nullptr < lhs;
	}
	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT>
		bool operator > (
			nullptr_t, 
			const shared_ptr<ObjectT, IncRefPolicy, DeleterT>& rhs) noexcept
	{
		return rhs < nullptr;
	}


	template<typename ObjectT,
	typename IncRefPolicy,
	typename DeleterT> 
	void swap(
		shared_ptr<ObjectT, IncRefPolicy, DeleterT>& lhs,
		shared_ptr<ObjectT, IncRefPolicy, DeleterT>& rhs) noexcept
	{
		lhs.swap(rhs);
	}

	namespace Internal
	{
		template<
			typename NewObjectT,
			typename DeleterT>
		struct DeleterRebind
		{
			using type = DeleterT;
		};

		template<
			typename NewObjectT,
			template<typename>
		class DeleterT,
			typename ObjectT>
		struct DeleterRebind<NewObjectT, DeleterT<ObjectT>>
		{
			using type = DeleterT<NewObjectT>;
		};

	}
	// 20.8.2.2.9, shared_ptr casts:
	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT,
		typename ObjectRhsT>
	shared_ptr<ObjectT, IncRefPolicy, DeleterT> 
		static_pointer_cast(const shared_ptr<ObjectRhsT, IncRefPolicy, DeleterT>& r) noexcept
	{
		shared_ptr<ObjectT, IncRefPolicy, DeleterT> ptr{ r,static_cast<ObjectT*>(r.get()) };
		return ptr;
	}
	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT,
		typename ObjectRhsT>
		shared_ptr<ObjectT, IncRefPolicy, DeleterT>
		dynamic_pointer_cast(const shared_ptr<ObjectRhsT, IncRefPolicy, DeleterT>& r) noexcept
	{
		auto rawPtr = dynamic_cast<ObjectT*>(r.get());
		if(rawPtr) 
			return shared_ptr<ObjectT, IncRefPolicy, DeleterT>{r, rawPtr};
		else 
			return shared_ptr<ObjectT, IncRefPolicy, DeleterT>();
	}

	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT,
		typename ObjectRhsT>
		auto
		const_pointer_cast(const shared_ptr<ObjectRhsT, IncRefPolicy, DeleterT>& r) noexcept
	{
		using NewDeleterType = typename Internal::DeleterRebind<ObjectT,DeleterT>::type;
		shared_ptr<ObjectT, IncRefPolicy, NewDeleterType> ptr{ r,const_cast<ObjectT*>(r.get()) };
		return ptr;
	}

	template<typename ObjectT,
		typename IncRefCountPolicy,
		typename DeleterT = default_delete<ObjectT>
	>
	class weak_ptr : public PtrBase<ObjectT,IncRefCountPolicy,DeleterT>
	{
	public:
		template<
			typename ObjectRhsT,
			typename IncRefPolicy,
			typename DeleterRhsT>
		friend class shared_ptr;

		using BaseType = PtrBase<ObjectT, IncRefCountPolicy, DeleterT>;
		using element_type = typename BaseType::element_type;
		using inc_ref_policy = typename BaseType::inc_ref_policy;

		template<typename OtherT,
			typename OtherD>
		using OtherSharedType = shared_ptr<OtherT, inc_ref_policy, OtherD>;
		template<typename OtherT,
			typename OtherD>
			using OtherWeakType = weak_ptr<OtherT, inc_ref_policy, OtherD>;

		constexpr weak_ptr() noexcept = default;

		weak_ptr(const weak_ptr& rhs)
		{
			this->ResetW(rhs);
		}

		template<
			typename OtherT,
			typename OtherD,
			typename = enable_if_t<
				is_convertible<OtherD, DeleterT>::value
				&& is_convertible<OtherT*, element_type*>::value >
		>
			weak_ptr(const OtherSharedType<OtherT,OtherD>& r) noexcept
		{
			this->ResetW(r);
		}
		
		weak_ptr(weak_ptr&& r) noexcept
			:BaseType(Yupei::move(r))
		{

		}

		template<
			typename OtherT,
			typename OtherD,
			typename = enable_if_t<
			is_convertible<OtherD, DeleterT>::value
			&& is_convertible<OtherT*, element_type*>::value >
		>
			weak_ptr(OtherWeakType<OtherT,OtherD>&& r) noexcept
			:BaseType(Yupei::move(r))
		{

		}
		// 20.8.2.3.2, destructor
		~weak_ptr()
		{
			this->DecWRef();
		}
		// 20.8.2.3.3, assignment
		weak_ptr& operator=(const weak_ptr & r) noexcept
		{
			weak_ptr(r).swap(*this);
			return *this;
		}
		template<
			typename OtherT,
			typename OtherD,
			typename = enable_if_t<
			is_assignable<DeleterT&, const OtherD& >::value
			&& is_convertible<OtherT*, element_type*>::value >
		>
		weak_ptr& operator=(const OtherWeakType<OtherT,OtherD>& r) noexcept
		{
			this->ResetW(r);
			return *this;
		}
		template<
			typename OtherT,
			typename OtherD,
			typename = enable_if_t<
			is_assignable<DeleterT&, const OtherD& >::value
			&& is_convertible<OtherT*, element_type*>::value >
		>
			weak_ptr& operator=(const OtherSharedType<OtherT,OtherD>& r) noexcept
		{
			this->ResetW(r);
			return *this;
		}
		weak_ptr& operator=(weak_ptr&& r) noexcept
		{
			weak_ptr(Yupei::move(r)).swap(*this);
			return *this;
		}
		template<
			typename OtherT,
			typename OtherD,
			typename = enable_if_t<
			is_assignable<DeleterT&, OtherD&& >::value
			&& is_convertible<OtherT*, element_type*>::value >
		>
		weak_ptr& operator=(OtherWeakType<OtherT,OtherD>&& r) noexcept
		{
			weak_ptr(Yupei::move(r)).swap(*this);
			return *this;
		}
		// 20.8.2.3.4, modifiers
		//void swap(weak_ptr& r) noexcept;
		void reset() noexcept
		{
			weak_ptr().swap(*this);
		}
		// 20.8.2.3.5, observers
		long use_count() const noexcept
		{
			return this->UseCount();
		}
		bool expired() const noexcept
		{
			return use_count() == 0;
		}
		OtherSharedType<ObjectT,DeleterT> lock() const
		{
			OtherSharedType<ObjectT, DeleterT> ptr{};
			ptr.Reset(*this,false);
			return ptr;
		}
	};

	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT>
		void swap(
			weak_ptr<ObjectT, IncRefPolicy, DeleterT>& lhs,
			weak_ptr<ObjectT, IncRefPolicy, DeleterT>& rhs) noexcept
	{
		lhs.swap(rhs);
	}

	namespace Internal
	{
		template<typename T>
		struct DeleterAddConst
		{
			using type = T;
		};
		template<
			template<typename>
		class T,
		typename ObjectT>
		struct DeleterAddConst<T<ObjectT>>
		{
			using type = T<const ObjectT>;
		};
	}

	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT>
	class enable_shared_from_this 
	{
	protected:
		template<typename ObjectT,
			typename IncRefPolicy,
			typename DeleterT>
		friend class shared_ptr;

		using SharedType = shared_ptr<ObjectT, IncRefPolicy, DeleterT>;
		using WeakType = weak_ptr<ObjectT, IncRefPolicy, DeleterT>;

		constexpr enable_shared_from_this() noexcept = default;
		enable_shared_from_this(enable_shared_from_this const&) noexcept = default;
		enable_shared_from_this& operator=(enable_shared_from_this const&)
		{
			return *this;
		}
		~enable_shared_from_this() = default;
	public:
		SharedType shared_from_this()
		{
			return SharedType(weakPtr);
		}

		using ReturnType = shared_ptr<const ObjectT, IncRefPolicy, typename Internal::DeleterAddConst<DeleterT>::type>;
		ReturnType shared_from_this() const
		{
			return ReturnType(weakPtr);
		}
	private:
		mutable WeakType weakPtr;
	};

	template<typename ObjectT,
		typename IncRefPolicy,
		typename DeleterT>
	template<typename ObjectRhsT,
		typename DeleterRhsT>
	void shared_ptr<ObjectT,IncRefPolicy,DeleterT>::DoEnable(const enable_shared_from_this<ObjectRhsT, IncRefPolicy, DeleterRhsT>* p) const
	{
		if (p)
		{
			p->weakPtr.ResetW(*this);
		}
	}

	

	template<typename ObjectT, typename D = default_delete<ObjectT>>
	using unsynchronized_shared_ptr = Yupei::shared_ptr<ObjectT, UnsynchronizedPolicy, D>;

	template<typename ObjectT, typename D = default_delete<ObjectT>>
	using unsynchronized_weak_ptr = Yupei::weak_ptr<ObjectT, UnsynchronizedPolicy, D>;

	template<typename ObjectT, typename DeleterT = default_delete<ObjectT>>
	using enable_unsynchronized_shared_from_this = Yupei::enable_shared_from_this<ObjectT, UnsynchronizedPolicy, DeleterT>;

	template<typename ObjectT, 
		typename DeleterT = default_delete<ObjectT>,
		typename... Args
	>
		unsynchronized_shared_ptr<ObjectT, DeleterT> make_unsynchronized_shared(Args&&... args)
	{
		return Yupei::make_shared<ObjectT, UnsynchronizedPolicy, DeleterT>(Yupei::forward<Args>(args)...);
	}
}


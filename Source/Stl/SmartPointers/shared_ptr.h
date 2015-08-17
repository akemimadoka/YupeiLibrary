#pragma once

#include "..\type_traits.h"
#include "..\utility_internal.h"
#include "..\__swap.h"
#include "..\compress_pair.h"
#include "default_delete.h"
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

	template<typename T, typename Inc, typename D>
	class weak_ptr;

	//This is an unsynchronized of std::shared_ptr called unsynchronized_ptr.
	//IncRefPolicy should have following member functions:
	//void IncreaseRefCount(std::ptrdiff_t&)
	//bool DecreaseRefCount(std::ptrdiff_t& ref_count) (after the decreasing,if ref_count == 0,return true)
	//bool TryIncreaseRefCount(std::ptrdiff_t& ref_count)
	//Before we write the real ptr,we need some contracts on IncRefPolicy.
	//Maybe we can't validate the signature because of the template functions.
	
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

	template<typename IncRefPolicy>
	struct RefCount
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
		bool DecRefCount()
		{
			return IncRefPolicy().DecreaseRefCount(refCount);
		}
		void IncWeakCount()
		{
			IncRefPolicy().IncreaseRefCount(weakCount);
		}
		bool DecWeakCount()
		{
			if (IncRefPolicy().DecreaseRefCount(weakCount))
			{
				delete this;
				return true;
			}
			return false;
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

	template<typename Type,
		typename IncRefCountPolicy,
		typename Deleter
		>
	class PtrBase
	{
	public:
		template<typename T,typename Inc,typename D>
		friend class PtrBase;
		template<typename T, typename Inc, typename D>
		friend class weak_ptr;
		using element_type = remove_extent_t<Type>;
		using ref_count_type = RefCount<IncRefCountPolicy>;
		using inc_ref_policy = IncRefCountPolicy;
		using deleter_type = Deleter;
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
			refCountObj = new ref_count_type();
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
		void ResetW(element_type* p, ref_count_type* _ref_count)
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
			{
				refCountObj->IncRefCount();
			}
		}
		bool TryIncRef()
		{
			return refCountObj->TryIncRefCount();
		}
		void DecRef()
		{
			if (refCountObj)
			{
				if (refCountObj->DecRefCount())
				{
					deleter_type()(rawPtr);
					rawPtr = nullptr;
					refCountObj->DecWeakCount();
				}
			}
		}
		void DecWRef()
		{
			if (refCountObj)
			{
				refCountObj->DecWeakCount();
			}
		}
		void IncWRef()
		{
			if (refCountObj)
			{
				refCountObj->IncWeakCount();
			}
		}
		std::size_t UseCount() const noexcept
		{
			if (refCountObj)
			{
				return refCountObj->GetRefCount();
			}
			return{};
		}
	protected:
		RefCount<IncRefCountPolicy>* refCountObj;
		element_type* rawPtr;
	};

	template<typename Type,
		typename IncRefCountPolicy,
		typename Deleter = default_delete<Type>
	>
	class shared_ptr : public PtrBase<Type, IncRefCountPolicy,Deleter>
	{
	public:
		template<typename T,typename Inc,typename D>
		friend class shared_ptr;
		using base_type = PtrBase<Type, IncRefCountPolicy, Deleter >;
		using element_type = typename base_type::element_type;
		using inc_policy = IncRefCountPolicy;
		using this_ptr_type = shared_ptr<Type, IncRefCountPolicy, Deleter>;
		template<typename OtherT,
			typename OtherD>
		using OtherPtrType = shared_ptr<OtherT, inc_policy, OtherD>;
		template<typename OtherT,
			typename OtherD>
		using OtherWeakPtrType = weak_ptr<OtherT, inc_policy, OtherD>;
		constexpr shared_ptr() noexcept = default;

		template<typename Y,
		typename = enable_if_t<
			is_convertible<Y*,element_type*>::value>
		>
		explicit shared_ptr(Y* p)
			:PtrBase(p)
		{
		}
		template<typename OtherT,
		typename OtherD,
		typename = enable_if_t<
			is_convertible<OtherD,Deleter>::value
			&& is_convertible<OtherT*,element_type*>::value>
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
				is_convertible<OtherD, Deleter>::value
				&& is_convertible<OtherT*, element_type*>::value >
		>
			shared_ptr(const OtherPtrType<OtherT,OtherD>& r) noexcept
		{
			this->Reset(r);
		}
		shared_ptr(shared_ptr&& r) noexcept
			:base_type(Yupei::move(r))
		{
			r.refCountObj = nullptr;
			r.rawPtr = nullptr;
		}
		template<typename OtherT,
			typename OtherD,
			typename = enable_if_t<
			is_convertible<OtherD, Deleter>::value
			&& is_convertible<OtherT*, element_type*>::value >
		>
			shared_ptr(OtherPtrType<OtherT, OtherD>&& r) noexcept
			:base_type(Yupei::move(r))
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
				&& is_convertible<OtherD,Deleter>::value>
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
			&& is_assignable<Deleter&,const OtherD&>::value>
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
			&& is_assignable<Deleter&, OtherD&&>::value>
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
	};

	template<typename Type,
		typename IncRefCountPolicy,
		typename Deleter = default_delete<Type>
	>
	class weak_ptr : public PtrBase<Type,IncRefCountPolicy,Deleter>
	{
	public:

		using base_type = PtrBase<Type, IncRefCountPolicy, Deleter>;
		using element_type = typename base_type::element_type;
		using inc_policy = IncRefCountPolicy;
		template<typename OtherT,
			typename OtherD>
		using OtherSharedType = shared_ptr<OtherT, inc_policy, OtherD>;
		template<typename OtherT,
			typename OtherD>
			using OtherWeakType = weak_ptr<OtherT, inc_policy, OtherD>;
		constexpr weak_ptr() noexcept = default;

		weak_ptr(const weak_ptr& rhs)
		{
			this->ResetW(rhs);
		}

		template<
			typename OtherT,
			typename OtherD,
			typename = enable_if_t<
				is_convertible<OtherD, Deleter>::value
				&& is_convertible<OtherT*, element_type*>::value >
		>
			weak_ptr(const OtherSharedType<OtherT,OtherD>& r) noexcept
		{
			this->ResetW(r);
		}
		
		weak_ptr(weak_ptr&& r) noexcept
			:base_type(Yupei::move(r))
		{
			//BUG!!!
			r.rawPtr = nullptr;
			r.refCountObj = nullptr;
		}

		template<
			typename OtherT,
			typename OtherD,
			typename = enable_if_t<
			is_convertible<OtherD, Deleter>::value
			&& is_convertible<OtherT*, element_type*>::value >
		>
			weak_ptr(OtherWeakType<OtherT,OtherD>&& r) noexcept
			:base_type(Yupei::move(r))
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
			is_assignable<Deleter&, const OtherD& >::value
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
			is_assignable<Deleter&, const OtherD& >::value
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
			is_assignable<Deleter&, OtherD&& >::value
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
		OtherSharedType<Type,Deleter> lock() const
		{
			OtherSharedType<Type, Deleter> ptr{};
			ptr.Reset(*this,false);
			return ptr;
		}
	};



	template<typename Type,typename D = default_delete<Type>>
	using unsynchronized_shared_ptr = Yupei::shared_ptr<Type, UnsynchronizedPolicy,D>;

	template<typename Type, typename D = default_delete<Type>>
	using unsynchronized_weak_ptr = Yupei::weak_ptr<Type, UnsynchronizedPolicy, D>;
}


#pragma once

#include "traits_internal.h"

namespace Yupei
{


	//Why ?
#define ADD_CDECL(FUNC,CV_OPT,REF_OPT)\
	FUNC(_cdecl,CV_OPT,REF_OPT)

	//https://msdn.microsoft.com/en-us/library/vstudio/b0084kay(v=vs.140).aspx
	//for Predefined Macros


	//_M_CEE
	// Defined for a compilation that uses any form of /clr
	// (/clr:oldSyntax, /clr:safe, for example).

#ifdef _M_CEE
#define ADD_CLRCALL(FUNC, CV_OPT, REF_OPT) \
	FUNC(__clrcall, CV_OPT, REF_OPT)

#else /* _M_CEE */
#define ADD_CLRCALL(FUNC, CV_OPT, REF_OPT)
#endif /* _M_CEE */

#if defined(_M_IX86) && !defined(_M_CEE)
#define ADD_FASTCALL(FUNC,CV_OPT,REF_OPT)\
	FUNC(__fastcall,CV_OPT,REF_OPT)
#else
	FUNC(__fastcall, CV_OPT, REF_OPT)
#endif

#if defined(_M_IX86)
#define ADD_STDCALL(FUNC,CV_OPT,REF_OPT)\
	FUNC(__stdcall,CV_OPT,REF_OPT)

#define ADD_THISCALL(FUNC,CV_OPT,REF_OPT)\
	FUNC(__thiscall,CV_OPT,REF_OPT)
#else
#define ADD_STDCALL(FUNC,CV_OPT,REF_OPT)
#define ADD_THISCALL(FUNC,CV_OPT,REF_OPT)
#endif

		// _M_IX86_FP 
		// Expands to an integer literal value indicating which /arch compiler option was used:
		// •0 if /arch:IA32 was used.
		// •1 if /arch:SSE was used.
		// •2 if /arch:SSE2, /arch:AVX or /arch:AVX2 was used. This value is the default if /arch was not specified. When /arch:AVX is specified, the macro __AVX__ is also defined. When /arch:AVX2 is specified, __AVX__ and __AVX2__ are also defined.
		// •See /arch (x86) for more information.

#if ((defined(_M_IX86) && _M_IX86_FP >= 2) \
	|| defined(_M_X64)) && !defined(_M_CEE)
#define ADD_VECTORCALL(FUNC, CV_OPT, REF_OPT) \
	FUNC(__vectorcall, CV_OPT, REF_OPT)

#else /* defined(_M_IX86) && _M_IX86_FP >= 2 etc. */
#define ADD_VECTORCALL(FUNC, CV_OPT, REF_OPT)
#endif /* defined(_M_IX86) && _M_IX86_FP >= 2 etc. */

#define ADD_CONV_TO_MEMBER_FUNC(FUNC, CV_OPT, REF_OPT) \
	ADD_CDECL(FUNC, CV_OPT, REF_OPT) \
	ADD_CLRCALL(FUNC, CV_OPT, REF_OPT) \
	ADD_FASTCALL(FUNC, CV_OPT, REF_OPT) \
	ADD_STDCALL(FUNC, CV_OPT, REF_OPT) \
	ADD_THISCALL(FUNC, CV_OPT, REF_OPT) \
	ADD_VECTORCALL(FUNC, CV_OPT, REF_OPT)

#define FUNCTION_ADAPTER(CALL_OPT,FUNC,REF_OPT)\
	FUNC(CALL_OPT)

#define ADD_CALL_OPT_TO_NONMEMBER_FUNC(FUNC) \
	ADD_CDECL(FUNCTION_ADAPTER, FUNC,) \
	ADD_CLRCALL(FUNCTION_ADAPTER, FUNC,) \
	ADD_FASTCALL(FUNCTION_ADAPTER, FUNC,) \
	ADD_STDCALL(FUNCTION_ADAPTER, FUNC,) \
	ADD_VECTORCALL(FUNCTION_ADAPTER, FUNC,)

#define ADD_CV_TO_MEMBER_FUNC(FUNC , REF_OPT)\
	ADD_CONV_TO_MEMBER_FUNC(FUNC, ,REF_OPT)\
	ADD_CONV_TO_MEMBER_FUNC(FUNC,const ,REF_OPT)\
	ADD_CONV_TO_MEMBER_FUNC(FUNC,volatile ,REF_OPT)\
	ADD_CONV_TO_MEMBER_FUNC(FUNC,const volatile ,REF_OPT)\


#define ADD_REF_CV_TO_MEMBER_FUNC(FUNC) \
	ADD_CV_TO_MEMBER_FUNC(FUNC, )\
	ADD_CV_TO_MEMBER_FUNC(FUNC, &)\
	ADD_CV_TO_MEMBER_FUNC(FUNC, &&)

#define ADD_CV_TO_CLASS(CLASS,REF_OPT)\
	CLASS(const,REF_OPT)\
	CLASS(,REF_OPT)\
	CLASS(volatile,REF_OPT)\
	CLASS(const volatile,REF_OPT)

#define ADD_CV_REF_TO_CLASS(CLASS)\
	ADD_CV_TO_CLASS(CLASS,)\
	ADD_CV_TO_CLASS(CLASS,&)\
	ADD_CV_TO_CLASS(CLASS,&&)


#define IS_MEMBER_FUNCTION_POINTER(CALL_OPT,CV_OPT,REF_OPT)\
	template<typename R,typename Class,typename... Args>\
	struct IsMemberFunctionHelper<R(CALL_OPT Class::*)(Args...)CV_OPT REF_OPT> \
{\
	using ClassType = Class;\
	using result_type = R;	\
	using BoolType = true_type;\
};
	namespace Internal
	{
		template<typename Type>
		struct IsMemberFunctionHelper
		{
			using BoolType = false_type;
		};

		ADD_REF_CV_TO_MEMBER_FUNC(IS_MEMBER_FUNCTION_POINTER)
	}
	
#define IS_MEMBER_FUNCTION_POINTER_ELLIPSIS(CV_OPT,REF_OPT)\
	template<typename R,typename Class,typename... Args>\
	struct IsMemberFunctionHelper<R(Class::*)(Args..., ...)CV_OPT REF_OPT> \
{\
	using ClassType = Class;\
	using result_type = R;	\
	using BoolType = true_type;\
};
	namespace Internal
	{
		ADD_CV_REF_TO_CLASS(IS_MEMBER_FUNCTION_POINTER_ELLIPSIS)
	}
		
#define IS_FUNCTION_(CALL_OPT)\
	template<typename R,typename... Args>\
	struct IsFunctionHelper<R CALL_OPT (Args...)>\
	{\
		using result_type = R;\
		using BoolType = true_type;\
	};

	namespace Internal
	{
		template<typename Type>
		struct IsFunctionHelper
		{
			using BoolType = false_type;
		};

		ADD_CALL_OPT_TO_NONMEMBER_FUNC(IS_FUNCTION_)
	}

	namespace Internal
	{
		template<typename R, typename... Args>
		struct IsFunctionHelper<R(Args..., ...)>
		{
			using result_type = R;
			using BoolType = true_type;
		};
	}
	
	namespace Internal
	{
		template<typename Type>
		struct IsMemberObjectPointerHelper
		{
			using BoolType = false_type;
		};

		template<typename Class, typename Member>
		struct IsMemberObjectPointerHelper<Member Class::*>
		{
			using ClassType = Class;
			using MemberType = Member;
			using BoolType = true_type;
		};

	}

	template<typename Type>
	using is_member_function_pointer = typename Internal::IsMemberFunctionHelper<Type>::BoolType;

	template<typename Type>
	using is_member_object_pointer = conditional_t<
		is_member_function_pointer<Type>::value,
		false_type,
		typename Internal::IsMemberObjectPointerHelper<Type>::BoolType
	>;
}
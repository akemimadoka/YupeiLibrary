#include "..\..\Source\Stl\memory.h"
#include <iostream>
#include <limits>

struct Foo { int i = 0; };

using namespace Yupei;

//http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4366.html
template <class T>
class deleter
{
	// count the number of times this deleter is used
	unsigned count_ = 0;
public:
	// special members
	~deleter() 
	{ 
		count_ = std::numeric_limits<unsigned>::max(); 
	}

	deleter() = default;

	deleter(const deleter&) {}

	deleter& operator=(const deleter&) { return *this; }

	deleter(deleter&& d)
		: count_(d.count_)
	{
		d.count_ = 0;
	}

	deleter& operator=(deleter&& d)
	{
		count_ = d.count_;
		d.count_ = 0;
		return *this;
	}

	// converting constructors
	template <class U,
	class = std::enable_if_t<
		std::is_convertible<U*, T*>::value >>
		deleter(const deleter<U>& d)
	{}

	template <class U,
	class = std::enable_if_t<std::is_convertible<U*, T*>::value >>
		deleter(deleter<U>&& d)
		: count_(d.count_)
	{
		d.count_ = 0;
	}

	// deletion operator
	void operator()(T* t)
	{
		delete t;
		++count_;
	}

	// observers
	unsigned count() const 
	{ 
		return count_; 
	}

	friend
		std::ostream&
		operator<<(std::ostream& os, const deleter& d)
	{
		return os << d.count_;
	}

	template <class> friend class deleter;
};

struct do_nothing
{
	template <class T>
	void operator()(T*) {}
};

template <class T>
using Ptr = unique_ptr<T, deleter<T>&>;

struct base { int i = 0; };

struct derived : base { int j = 0; };

struct Base : enable_shared_from_this<Base>
{
	Base()
	{
	}

	unique_ptr<int> pi = make_unique<int>();

	virtual ~Base()
	{
		std::cout << "destructor called\n";
	}
};
struct Derived : Base
{
	unique_ptr<int> pi2 = make_unique<int>();
};

int main()
{
	{
		auto ptr = make_unique<int>(3);
		std::cout << *ptr;
		auto rptr = ptr.release();
		default_delete<int>{}(rptr);
	}
	{
		auto ptr = make_unique<int[]>(3);
		std::cout << ptr[1];
	}
	{
		//unique_ptr<Foo const * const[]> ptr1(new Foo*[10]);
	}
	{
		int i = 0;
		unique_ptr<int, do_nothing> p1(&i);
		unique_ptr<int> p2;
		p1 != p2;
		auto p3 = move(p1);
		assert(p1 == nullptr);
		// This mistakenly compiles:(Mine is OK!!!!!)
		//static_assert(is_assignable<decltype(p2), decltype(p1)>::value, "hahah");

		//But this correctly does not compile:
	    //p2 = std::move(p1);
	}
	{
		

		deleter<base> db;
		deleter<derived> dd;
		std::cout << "db = " << db << '\n';
		std::cout << "dd = " << dd << '\n';
		{
			Ptr<derived> pd(new derived, dd);
			pd.reset(new derived);
			Ptr<base> pb(nullptr, db);
			pb = move(pd);  // The converting move assignment!
			std::cout << "pb.get_deleter() = " << pb.get_deleter() << '\n';
			std::cout << "pd.get_deleter() = " << pd.get_deleter() << '\n';
			swap(pd, pd);
		}
		std::cout << "db = " << db << '\n';
		std::cout << "dd = " << dd << '\n';

	}
	{
		auto print_helper = [](auto& p1, auto& p2)
		{
			std::cout << p1.use_count() << '\n';
			std::cout << p2.use_count() << '\n';
		};
		shared_ptr<int> sp{ new int() };
		auto rpd = new Derived();
		shared_ptr<Base> pb{ rpd };
		shared_ptr<Derived> pd{ new Derived() };
		print_helper(pb, pd);
		pb = move(pd);
		print_helper(pb, pd);
		weak_ptr<Base> wptr{ pb };
		std::cout << "wptr's " << wptr.use_count() << '\n';
		auto npb = wptr.lock();
		std::cout << "npb's " << npb.use_count() << '\n';
	}
	{
		auto spb = make_shared<const Base>();//1
		auto spb2 = spb->shared_from_this();//2
		auto spb6 = const_pointer_cast<Base>(spb2);//3
		shared_ptr<Base> spb3 = make_shared<Derived>();//1
		auto spb4 = spb3->shared_from_this();//2
		auto spb5 = dynamic_pointer_cast<Derived>(spb3);//3
		std::cout << spb4.use_count() << '\n';
		std::cout << (spb == spb2) << '\n';
		//compile error
		//spb[1];
		
	}
	{
		std::cout << '\n';
		auto sp = make_shared<int[]>(4);
		auto sp2 = make_shared<int[4]>();
		auto sp3 = make_shared<Base[4]>();
		//compile error
		//sp.operator->();
	}
	getchar();
	//_CrtDumpMemoryLeaks();
	return 0;
}
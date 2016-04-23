#include <OS\Windows\NativeHandles.hpp>
#include <UnitTest.hpp>

TEST_CASE(FileCloserTest)
{
	using namespace Yupei;
	FileHandle f;
	TEST_ASSERT(f.Get() == FileHandle::InvalidHandle());
}
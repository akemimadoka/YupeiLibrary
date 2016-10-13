#include <OS/Windows/NativeHandles.hpp>
#include <catch.hpp>

TEST_CASE("FileCloser")
{
	using namespace Yupei;
	FileHandle f;
	CHECK(f.Get() == FileHandle::InvalidHandle());
}
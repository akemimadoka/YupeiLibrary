#define _CRTDBG_MAP_ALLOC
#define CATCH_CONFIG_RUNNER

#include <crtdbg.h>
#include <catch.hpp>

int main(int argc, char * argv[]) 
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	return Catch::Session().run(argc, argv);
}
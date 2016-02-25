#include "UnitTest.hpp"
#include <iostream>

namespace Yupei
{
    namespace Internal
    {
        void UnitTest::RunTest()
        {
            for (auto&& t : tests_)
            {
                try
                {
                    t.second();
                }
                catch (UnitTestException& e)
                {
                    log_.push_back(std::string {e.what()} +" at line " + std::to_string(e.LineNumber_) + " in file " + std::string {e.FileName_});
                }
                catch (...)
                {

                }
            }
            PrintInfo();
        }

        void UnitTest::AddTest(const char* str, const std::function<void()>& func)
        {
            tests_.insert({std::string{str}, func});
        }

        void UnitTest::PrintInfo()
        {
            for (const auto& message : log_)
                std::cerr << message << "\n";
        }

        UnitTest& GetUnitTest() 
        {
            static UnitTest unitTest;
            return unitTest;
        }
    }
    
}


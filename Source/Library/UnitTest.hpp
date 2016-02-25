#pragma once

#include "Extensions.hpp"
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>

namespace Yupei
{
    namespace Internal
    {
        class UnitTestException : public std::runtime_error
        {
        public:
            UnitTestException(const char* str, std::uint32_t lineNumber, const char* fileName)
                :runtime_error{str},
                LineNumber_{lineNumber},
                FileName_{fileName}
            {}

            UnitTestException(const std::string& str, std::uint32_t lineNumber, const char* fileName)
                :UnitTestException{str.c_str(), lineNumber, fileName}
            {}

            std::uint32_t LineNumber_;
            const char* FileName_;
        };

#define UNIT_TEST_NS ::Yupei::Internal
#define TEST_ASSERT(Cond) do{ if(!(Cond)) throw UNIT_TEST_NS::UnitTestException{"UnitTest Failed!", __LINE__, __FILE__};} while(0)

        class UnitTest
        {
        public:
            void RunTest();
            void AddTest(const char* str, const std::function<void()>& func);
            void PrintInfo();

        private:
            std::unordered_map<std::string, std::function<void()>> tests_;
            std::vector<std::string> log_;
            UnitTest() = default;

            friend UnitTest& GetUnitTest();
        };

        UnitTest& GetUnitTest();

#define UNIT_TEST_DECLARE_CLASS(className, funcName, func)                                                                              \
        struct className                                                                                                                \
        {                                                                                                                               \
            className()                                                                                                                 \
            {                                                                                                                           \
                UNIT_TEST_NS::GetUnitTest().AddTest(funcName, func);                                                                    \
            }                                                                                                                           \
        }TOKEN_PASTE(className, _INSTANCE)                                                                                                         

#define TEST_CASE(funcName)                                                                                                             \
        void funcName();                                                                                                                \
        UNIT_TEST_DECLARE_CLASS(TOKEN_PASTE(TOKEN_PASTE(UNIT_TEST_CLASS_, __COUNTER__), funcName), STR(funcName), funcName);                                   \
        void funcName()

#define TEST_CASE_AN(func)                                                                                                              \
        UNIT_TEST_DECLARE_CLASS(TOKEN_PASTE(UNIT_TEST_CLASS_, __COUNTER__), "", func);
    }
}

#ifdef UNIT_TEST_MAIN

int main()
{
    UNIT_TEST_NS::GetUnitTest().RunTest();
    std::getchar();
    std::getchar();
}

#endif // UNIT_TEST_MAIN


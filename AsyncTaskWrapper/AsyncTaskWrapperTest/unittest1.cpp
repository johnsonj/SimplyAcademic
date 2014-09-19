#include "stdafx.h"
#include "CppUnitTest.h"
#include "AsyncTaskWrapper.h"

#include <thread>
#include <chrono>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AsyncTaskWrapperTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(GetResultBlockingOutput)
		{
			AsyncTaskWrapper<int> atw([]() -> int { return 42; });
			Assert::AreEqual(atw.GetResultBlocking(), 42);
		}
		TEST_METHOD(GetResultBlockingInputOutput)
		{
			AsyncTaskWrapper<int,int> atw([](int x) -> int { return x; }, 42);
			Assert::AreEqual(atw.GetResultBlocking(), 42);
		}
		// Test a non-blocking run
		// This test is likely to cause issues because it relies on time
		TEST_METHOD(GetResultsNonBlocking) 
		{
			struct {
				int operator()(int length) {
					std::this_thread::sleep_for(std::chrono::milliseconds{ length });
					return length;
				}
			} sleep;

			AsyncTaskWrapper<int, int> atw(sleep, 500);

			Assert::IsFalse(atw.IsRunning());
			atw.Start();
			Assert::IsTrue(atw.IsRunning());

			sleep(100);
			Assert::IsFalse(atw.IsDone());

			sleep(1000);
			Assert::IsTrue(atw.IsDone());
			Assert::AreEqual(atw.GetResult(), 500);
		}

	};
}
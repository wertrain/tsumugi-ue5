#include "pch.h"
#include "CppUnitTest.h"
#include "Foundation/Types.h"
#include "Script/Lexing/LexingStringReader.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(UnitTest)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			std::string name = "Bill";
			Assert::AreEqual(name.c_str(), "Bill");
		}
	};
}

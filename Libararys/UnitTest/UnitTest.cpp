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
		
		TEST_METHOD(TestLexingStringReader)
		{
			tsumugi::script::lexing::LexingStringReader reader;
			reader.SetString(L"���{��̃e�X�g�B����̓e�X�g�ł��B");
			Assert::AreEqual(reader.Peek(), L'��');
			Assert::AreNotEqual(reader.Peek(), L'�{');

			Assert::AreEqual(reader.Read(), L'��');
			Assert::AreEqual(reader.Read(), L'�{');
			Assert::AreEqual(reader.Read(), L'��');
		}
	};
}

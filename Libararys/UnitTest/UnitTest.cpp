#include "pch.h"
#include "CppUnitTest.h"
#include "Foundation/Types.h"
#include "Script/Lexing/ScriptLexer.h"
#include "Script/Lexing/ScriptToken.h"
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

		TEST_METHOD(Token)
		{
			tsumugi::script::lexing::Lexer lexer(TT("  \t +  "));
			tsumugi::script::lexing::Token* token = lexer.NextToken();
			Assert::IsTrue(token->GetTokenType() == tsumugi::script::lexing::TokenType::kPlus);
			delete(token);
		}
	};
}

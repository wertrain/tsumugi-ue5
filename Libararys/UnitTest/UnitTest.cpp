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
			reader.SetString(L"日本語のテスト。これはテストです。");
			Assert::AreEqual(reader.Peek(), L'日');
			Assert::AreNotEqual(reader.Peek(), L'本');

			Assert::AreEqual(reader.Read(), L'日');
			Assert::AreEqual(reader.Read(), L'本');
			Assert::AreEqual(reader.Read(), L'語');
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

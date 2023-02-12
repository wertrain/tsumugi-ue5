#include "pch.h"
#include "CppUnitTest.h"
#include "Foundation/Types.h"
#include "Script/Lexing/ScriptLexer.h"
#include "Script/Lexing/ScriptToken.h"
#include "Script/Lexing/LexingStringReader.h"
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define MSG(msg) [&]{ std::wstringstream _s; _s << msg; return _s.str(); }().c_str()

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

		TEST_METHOD(TokenList)
		{
			auto input = TT("=+-(){},;[]<>>=<===");

			std::vector<tsumugi::script::lexing::Token*> testTokens;
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kAssign, TT("=")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kPlus, TT("+")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kMinus, TT("-")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kLeftParenthesis, TT("(")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kRightParenthesis, TT(")")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kLeftBraces, TT("{")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kRightBraces, TT("}")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kComma, TT(",")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kSemicolon, TT(";")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kLeftBrackets , TT("[")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kRightBrackets, TT("]")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kLessThan, TT("<")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kGreaterThan, TT(">")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kGreaterThanOrEqual, TT(">=")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kLessThanOrEqual , TT("<=")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kEqual, TT("==")));

			tsumugi::script::lexing::Lexer lexer(input);
			for each (const auto* testToken in testTokens)
			{
				const auto* nextToken = lexer.NextToken();
				Assert::IsNotNull(nextToken);
				Assert::AreEqual(testToken->GetTokenType() == nextToken->GetTokenType(), true);
				//Assert::AreEqual(testToken->GetLiteral().compare(nextToken->GetLiteral()), 0);
			}

			for each (auto token in testTokens)
			{
				delete(token);
			}
			testTokens.clear();
		}

		TEST_METHOD(Token)
		{
			tsumugi::script::lexing::Lexer lexer(TT("  \t +  "));
			tsumugi::script::lexing::Token* token = lexer.NextToken();
			Assert::IsTrue(token->GetTokenType() == tsumugi::script::lexing::TokenType::kPlus);
			delete(token);
		}

		TEST_METHOD(SimpleExpression)
		{
			auto input = TT("let five = 5;");
			tsumugi::script::lexing::Lexer lexer(input);

			std::vector<tsumugi::script::lexing::Token*> testTokens;
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kLet, TT("let")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kIdentifier, TT("five")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kAssign, TT("=")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kInteger, TT("5")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kSemicolon, TT(";")));

			for each (const auto * testToken in testTokens)
			{
				const auto* nextToken = lexer.NextToken();
				Assert::IsNotNull(nextToken);
				Assert::AreEqual(testToken->GetTokenType() == nextToken->GetTokenType(), true, 
					MSG("Expected " << tsumugi::script::lexing::TokenTypeToString(testToken->GetTokenType()) << " Actual:" << tsumugi::script::lexing::TokenTypeToString(nextToken->GetTokenType())));
				//Assert::AreEqual(testToken->GetLiteral().compare(nextToken->GetLiteral()), 0);
			}
		}
	};
}

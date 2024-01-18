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
	TEST_CLASS(LexingUnitTest)
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
			Assert::AreEqual(reader.Read(), L'の');
			Assert::AreEqual(reader.Read(), L'テ');
			Assert::AreEqual(reader.Read(), L'ス');
			Assert::AreEqual(reader.Read(), L'ト');
			Assert::AreEqual(reader.Read(), L'。');
			Assert::AreEqual(reader.Read(), L'こ');
			Assert::AreEqual(reader.Read(), L'れ');
			Assert::AreEqual(reader.Read(), L'は');
			Assert::AreEqual(reader.Read(), L'テ');
			Assert::AreEqual(reader.Read(), L'ス');
			Assert::AreEqual(reader.Read(), L'ト');
			Assert::AreEqual(reader.Read(), L'で');
			Assert::AreEqual(reader.Read(), L'す');
			Assert::AreEqual(reader.Read(), L'。');
			Assert::AreEqual(reader.Read(), (tchar)-1);
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
				delete testToken;
			}
			testTokens.clear();
		}

		TEST_METHOD(TokenWithWhiteSpace)
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
				delete nextToken;
			}
			testTokens.clear();
		}

		TEST_METHOD(SimpleCode)
		{
			std::wstring simplecode =
				L"let five = 5;"
				L"let ten = 10;"
				L""
				L"let add = function(x, y) {"
				L"    x + y;"
				L"};"
				L""
				L"let result = add(five, ten);";

			std::vector<tsumugi::script::lexing::Token*> testTokens;
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kLet, TT("let")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kIdentifier, TT("five")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kAssign, TT("=")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kInteger, TT("5")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kSemicolon, TT(";")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kLet, TT("let")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kIdentifier, TT("ten")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kAssign, TT("=")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kInteger, TT("10")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kSemicolon, TT(";")));

			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kLet, TT("let")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kIdentifier, TT("add")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kAssign, TT("=")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kFunction, TT("function")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kLeftParenthesis, TT("(")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kIdentifier, TT("x")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kComma, TT(",")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kIdentifier, TT("y")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kRightParenthesis, TT(")")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kLeftBraces, TT("{")));

			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kIdentifier, TT("x")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kPlus, TT("+")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kIdentifier, TT("y")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kSemicolon, TT(";")));

			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kRightBraces, TT("}")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kSemicolon, TT(";")));

			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kLet, TT("let")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kIdentifier, TT("result")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kAssign, TT("=")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kIdentifier, TT("add")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kLeftParenthesis, TT("(")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kIdentifier, TT("five")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kComma, TT(",")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kIdentifier, TT("ten")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kRightParenthesis, TT(")")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kSemicolon, TT(";")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kEOF, TT_EMPTY()));

			tsumugi::script::lexing::Lexer lexer(simplecode.c_str());
			for each (const auto * testToken in testTokens)
			{
				const auto* nextToken = lexer.NextToken();
				Assert::IsNotNull(nextToken);
				Assert::AreEqual(testToken->GetTokenType() == nextToken->GetTokenType(), true,
					MSG("Expected " << tsumugi::script::lexing::TokenTypeToString(testToken->GetTokenType()) << " Actual:" << tsumugi::script::lexing::TokenTypeToString(nextToken->GetTokenType())));
				delete nextToken;
			}
			testTokens.clear();
		}

		TEST_METHOD(SimpleCodeIfAndBool)
		{
			std::wstring simplecode =
				L"if (5 < 10) {"
				L"    return true;"
				L"} else {"
				L"    return false;"
				L"}";

			std::vector<tsumugi::script::lexing::Token*> testTokens;
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kIf, TT("if")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kLeftParenthesis, TT("(")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kInteger, TT("5")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kLessThan, TT("<")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kInteger, TT("10")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kRightParenthesis, TT(")")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kLeftBraces, TT("{")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kReturn, TT("return")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kTrue, TT("true")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kSemicolon, TT(";")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kRightBraces, TT("}")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kElse, TT("else")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kLeftBraces, TT("{")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kReturn, TT("return")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kFalse, TT("false")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kSemicolon, TT(";")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kRightBraces, TT("}")));
			testTokens.push_back(new tsumugi::script::lexing::Token(tsumugi::script::lexing::TokenType::kEOF, TT_EMPTY()));

			tsumugi::script::lexing::Lexer lexer(simplecode.c_str());
			for each (const auto * testToken in testTokens)
			{
				const auto* nextToken = lexer.NextToken();
				Assert::IsNotNull(nextToken);
				Assert::AreEqual(testToken->GetTokenType() == nextToken->GetTokenType(), true,
					MSG("Expected " << tsumugi::script::lexing::TokenTypeToString(testToken->GetTokenType()) << " Actual:" << tsumugi::script::lexing::TokenTypeToString(nextToken->GetTokenType())));
				delete nextToken;
			}
			testTokens.clear();
		}
	};
}

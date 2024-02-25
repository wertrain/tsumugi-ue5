#include "pch.h"
#include "CppUnitTest.h"
#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/Root.h"
#include "Script/AbstractSyntaxTree/Expressions/Identifier.h"
#include "Script/AbstractSyntaxTree/Expressions/IntegerLiteral.h"
#include "Script/AbstractSyntaxTree/Statements/LetStatement.h"
#include "Script/AbstractSyntaxTree/Statements/ReturnStatement.h"
#include "Script/AbstractSyntaxTree/Statements/ExpressionStatement.h"
#include "Script/Lexing/ScriptLexer.h"
#include "Script/Lexing/ScriptToken.h"
#include "Script/Lexing/ScriptLexer.h"
#include "Script/Parsing/ScriptParser.h"
#include "Script/Lexing/LexingStringReader.h"
#include "Log/TextLogger.h"
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define MSG(msg) [&]{ std::wstringstream _s; _s << msg; return _s.str(); }().c_str()

class TestConsole : public tsumugi::log::ILogConsole {
public:
	void WriteLine(const tsumugi::log::tlogchar* text) {
		Logger::WriteMessage(text);
		Logger::WriteMessage(tsumugi::log::tlognewline);
	}
} s_Console;

namespace UnitTest
{
	TEST_CLASS(LexingUnitTest)
	{
	public:
		
		TEST_METHOD(TestLexingStringReader)
		{
			tsumugi::script::lexing::LexingStringReader reader;
			reader.SetString(TT("日本語のテスト。これはテストです。"));
			Assert::AreEqual(reader.Peek(), TT('日'));
			Assert::AreNotEqual(reader.Peek(), TT('本'));

			Assert::AreEqual(reader.Read(), TT('日'));
			Assert::AreEqual(reader.Read(), TT('本'));
			Assert::AreEqual(reader.Read(), TT('語'));
			Assert::AreEqual(reader.Read(), TT('の'));
			Assert::AreEqual(reader.Read(), TT('テ'));
			Assert::AreEqual(reader.Read(), TT('ス'));
			Assert::AreEqual(reader.Read(), TT('ト'));
			Assert::AreEqual(reader.Read(), TT('。'));
			Assert::AreEqual(reader.Read(), TT('こ'));
			Assert::AreEqual(reader.Read(), TT('れ'));
			Assert::AreEqual(reader.Read(), TT('は'));
			Assert::AreEqual(reader.Read(), TT('テ'));
			Assert::AreEqual(reader.Read(), TT('ス'));
			Assert::AreEqual(reader.Read(), TT('ト'));
			Assert::AreEqual(reader.Read(), TT('で'));
			Assert::AreEqual(reader.Read(), TT('す'));
			Assert::AreEqual(reader.Read(), TT('。'));
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
			for (const auto* testToken : testTokens)
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

			for (const auto * testToken : testTokens)
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
			tstring simplecode =
				TT("let five = 5;")
				TT("let ten = 10;")
				TT("")
				TT("let add = function(x, y) {")
				TT("    x + y;")
				TT("};")
				TT("")
				TT("let result = add(five, ten);");

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
			for (const auto * testToken : testTokens)
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
			tstring simplecode =
				TT("if (5 < 10) {")
				TT("    return true;")
				TT("} else {")
				TT("    return false;")
				TT("}");

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
			for (const auto * testToken : testTokens)
			{
				const auto* nextToken = lexer.NextToken();
				Assert::IsNotNull(nextToken);
				Assert::AreEqual(testToken->GetTokenType() == nextToken->GetTokenType(), true,
					MSG("Expected " << tsumugi::script::lexing::TokenTypeToString(testToken->GetTokenType()) << " Actual:" << tsumugi::script::lexing::TokenTypeToString(nextToken->GetTokenType())));
				delete nextToken;
			}
			testTokens.clear();
		}

		TEST_METHOD(LogTest)
		{
			class TestConsole : public tsumugi::log::ILogConsole {
			public:
				void WriteLine(const tsumugi::log::tlogchar* text) {
					Logger::WriteMessage(text);
				}
			};
			tsumugi::log::TextLogger logger;
			logger.SetLogConsole(&s_Console);
			logger << "override << " << " test ";
			logger.Log(tsumugi::log::TextLogger::Categories::Information, "a: ", 10);
		}

		TEST_METHOD(TestLetStatement)
		{
			tstring letcode =
				TT("let x = 5;")
				TT("let y = 10;")
				TT("let xyz = 838383;");

			auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(letcode.c_str()));
			auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
			auto root = parser->ParseProgram();

			Assert::AreEqual(root->GetStatementCount() == 3, true,
				MSG("The number of Root.Statements is incorrect."));
			
			const tchar* tests[] = { TT("x"), TT("y"), TT("xyz") };
			for (int index = 0; index < 3; ++index)
			{
				const auto statement = root->GetStatement(index);
				Assert::AreEqual(
					statement->TokenLiteral().compare(TT("let")) == 0, true);
				Assert::IsNotNull(statement);
				const auto letStatement = static_cast<const tsumugi::script::ast::statement::LetStatement*>(statement);
				Assert::AreEqual(
					letStatement->TokenLiteral().compare(TT("let")) == 0, true);
				Assert::AreEqual(
					letStatement->GetName()->GetValue().compare(tests[index]) == 0, true);
				Assert::AreEqual(
					letStatement->GetName()->TokenLiteral().compare(tests[index]) == 0, true);
			}
		}

		TEST_METHOD(TestLetStatementError)
		{
			tstring letcode =
				TT("let x 5;")
				TT("let = 10;")
				TT("let;");

			auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(letcode.c_str()));
			auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
			parser->GetLogger().SetLogConsole(&s_Console);
			auto root = parser->ParseProgram();

			//Assert::AreEqual(root->GetStatementCount() == 3, true,
			//	MSG("The number of Root.Statements is incorrect."));
		}

		TEST_METHOD(TestReturnStatement)
		{
			tstring letcode =
				TT("return 5;")
				TT("return 10;")
				TT("return = 993322;");

			auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(letcode.c_str()));
			auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
			auto root = parser->ParseProgram();

			Assert::AreEqual(root->GetStatementCount() == 3, true,
				MSG("The number of Root.Statements is incorrect."));

			const tchar* tests[] = { TT("x"), TT("y"), TT("xyz") };
			for (int index = 0; index < 3; ++index)
			{
				const auto statement = root->GetStatement(index);
				Assert::AreEqual(
					statement->TokenLiteral().compare(TT("return")) == 0, true);
				Assert::IsNotNull(statement);
				const auto returnStatement = static_cast<const tsumugi::script::ast::statement::ReturnStatement*>(statement);
				Assert::AreEqual(
					returnStatement->TokenLiteral().compare(TT("return")) == 0, true);
			}
		}

		TEST_METHOD(TestIdentifierExpression)
		{
			tstring letcode =
				TT("foobar;");

			auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(letcode.c_str()));
			auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
			parser->GetLogger().SetLogConsole(&s_Console);
			auto root = parser->ParseProgram();

			Assert::AreEqual(root->GetStatementCount() == 1, true,
				MSG("The number of Root.Statements is incorrect."));

			//Assert::AreEqual(typeid(root->GetStatement(0)) == typeid(tsumugi::script::ast::statement::ExpressionStatement), true,
			//	MSG("Root.Statements(0) is not ExpressionStatement."));

			auto* statement = static_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
			//Assert::AreEqual(typeid(statement->GetExpression()) == typeid(tsumugi::script::ast::expression::Identifier), true,
			//	MSG("Statements.Expression is not Identifier."));

			auto* identifier = static_cast<const tsumugi::script::ast::expression::Identifier*>(statement->GetExpression());
			Assert::AreEqual(identifier->GetValue().compare(TT("foobar")) == 0, true,
				MSG("identifier.Value is incorrect."));

			Assert::AreEqual(identifier->TokenLiteral().compare(TT("foobar")) == 0, true,
				MSG("identifier.Token is incorrect."));
		}

		TEST_METHOD(TestIntegerExpression)
		{
			tstring letcode =
				TT("123;");

			auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(letcode.c_str()));
			auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
			parser->GetLogger().SetLogConsole(&s_Console);
			auto root = parser->ParseProgram();

			Assert::AreEqual(root->GetStatementCount() == 1, true,
				MSG("The number of Root.Statements is incorrect."));

			//Assert::AreEqual(typeid(root->GetStatement(0)) == typeid(tsumugi::script::ast::statement::ExpressionStatement), true,
			//	MSG("Root.Statements(0) is not ExpressionStatement."));

			auto* statement = static_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
			//Assert::AreEqual(typeid(statement->GetExpression()) == typeid(tsumugi::script::ast::expression::IntegerLiteral), true,
			//	MSG("Statements.Expression is not Identifier."));

			auto* integer = static_cast<const tsumugi::script::ast::expression::IntegerLiteral*>(statement->GetExpression());
			Assert::AreEqual(integer->GetValue() == 123, true,
				MSG("integer.Value is incorrect."));
		}
	};
}

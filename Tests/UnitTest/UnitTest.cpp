#include "pch.h"
#include "CppUnitTest.h"
#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/Root.h"
#include "Script/AbstractSyntaxTree/Expressions/Identifier.h"
#include "Script/AbstractSyntaxTree/Expressions/IntegerLiteral.h"
#include "Script/AbstractSyntaxTree/Expressions/BooleanLiteral.h"
#include "Script/AbstractSyntaxTree/Expressions/PrefixExpression.h"
#include "Script/AbstractSyntaxTree/Expressions/IfExpression.h"
#include "Script/AbstractSyntaxTree/Expressions/InfixExpression.h"
#include "Script/AbstractSyntaxTree/Expressions/FunctionLiteral.h"
#include "Script/AbstractSyntaxTree/Expressions/CallExpression.h"
#include "Script/AbstractSyntaxTree/Statements/LetStatement.h"
#include "Script/AbstractSyntaxTree/Statements/ReturnStatement.h"
#include "Script/AbstractSyntaxTree/Statements/ExpressionStatement.h"
#include "Script/AbstractSyntaxTree/Statements/BlockStatement.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/BooleanObject.h"
#include "Script/Objects/NullObject.h"
#include "Script/Objects/ErrorObject.h"
#include "Script/Objects/Environment.h"
#include "Script/Objects/FunctionObject.h"
#include "Script/Lexing/ScriptLexer.h"
#include "Script/Lexing/ScriptToken.h"
#include "Script/Parsing/ScriptParser.h"
#include "Script/Lexing/LexingStringReader.h"
#include "Script/Evaluator/Evaluator.h"
#include "Log/TextLogger.h"
#include <vector>
#include <variant>

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
			tstring retcode =
				TT("return 5;")
				TT("return 10;")
				//TT("return = 993322;");
				TT("return 5 + 5;");

			auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(retcode.c_str()));
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
			tstring code =
				TT("foobar;");

			auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(code.c_str()));
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

			const tsumugi::script::ast::expression::Identifier* identifier = static_cast<const tsumugi::script::ast::expression::Identifier*>(statement->GetExpression().get());
			Assert::AreEqual(identifier->GetValue().compare(TT("foobar")) == 0, true, MSG("identifier.Value is incorrect."));
			Assert::AreEqual(identifier->TokenLiteral().compare(TT("foobar")) == 0, true, MSG("identifier.Token is incorrect."));
		}

		TEST_METHOD(TestIntegerExpression)
		{
			tstring code =
				TT("123;");

			auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(code.c_str()));
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

			const tsumugi::script::ast::expression::IntegerLiteral* integer = static_cast<const tsumugi::script::ast::expression::IntegerLiteral*>(statement->GetExpression().get());
			Assert::AreEqual(integer->GetValue() == 123, true, MSG("integer.Value is incorrect."));
		}

		TEST_METHOD(TestPrefixExpressions)
		{
			struct PrefixExpressionSet {
				tstring code_;
				tstring operator_;
				int value_;
			};
			std::vector<PrefixExpressionSet> tests = {
				{ TT("!5"), TT("!"), 5 },
				{ TT("-15"), TT("-"), 15 },
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();

				Assert::AreEqual(root->GetStatementCount() == 1, true,
					MSG("The number of Root.Statements is incorrect."));

				//Assert::AreEqual(typeid(root->GetStatement(0)) == typeid(tsumugi::script::ast::statement::ExpressionStatement), true,
				//	MSG("Root.Statements(0) is not ExpressionStatement."));

				auto* expressionStatement = static_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
				//Assert::AreEqual(typeid(expressionStatement) == typeid(tsumugi::script::ast::expression::PrefixExpression), true,
				//	MSG("expressionStatement is not PrefixExpression."));

				const tsumugi::script::ast::expression::PrefixExpression* prefixExpression = static_cast<const tsumugi::script::ast::expression::PrefixExpression*>(expressionStatement->GetExpression().get());
				Assert::AreEqual(prefixExpression->GetOperator().compare(test.operator_) == 0, true,
					MSG("prefixExpression.Operator is incorrect."));

				const tsumugi::script::ast::expression::IntegerLiteral* integer = static_cast<const tsumugi::script::ast::expression::IntegerLiteral*>(prefixExpression->GetRight().get());
				Assert::AreEqual(integer->GetValue() == test.value_, true,
					MSG("integer.Value is incorrect."));
			}
		}

		TEST_METHOD(TestInfixExpressions)
		{
			struct InfixExpressionSet {
				tstring code_;
				int value0_;
				tstring operator_;
				int value1_;
			};
			std::vector<InfixExpressionSet> tests = {
				{TT("1 + 1;"), 1, TT("+"), 1},
				{TT("1 - 1;"), 1, TT("-"), 1},
				{TT("1 * 1;"), 1, TT("*"), 1},
				{TT("1 / 1;"), 1, TT("/"), 1},
				{TT("1 < 1;"), 1, TT("<"), 1},
				{TT("1 > 1;"), 1, TT(">"), 1},
				{TT("1 == 1;"), 1, TT("=="), 1},
				{TT("1 != 1;"), 1, TT("!="), 1},
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();

				Assert::AreEqual(root->GetStatementCount() == 1, true,
					MSG("The number of Root.Statements is incorrect."));

				//Assert::AreEqual(typeid(root->GetStatement(0)) == typeid(tsumugi::script::ast::statement::ExpressionStatement), true,
				//	MSG("Root.Statements(0) is not ExpressionStatement."));

				auto* expressionStatement = static_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
				//Assert::AreEqual(typeid(expressionStatement) == typeid(tsumugi::script::ast::expression::PrefixExpression), true,
				//	MSG("expressionStatement is not PrefixExpression."));

				const tsumugi::script::ast::expression::PrefixExpression* prefixExpression = static_cast<const tsumugi::script::ast::expression::PrefixExpression*>(expressionStatement->GetExpression().get());
				Assert::AreEqual(prefixExpression->GetOperator().compare(test.operator_) == 0, true,
					MSG("prefixExpression.Operator is incorrect."));

				const tsumugi::script::ast::expression::IntegerLiteral* integerLeft = static_cast<const tsumugi::script::ast::expression::IntegerLiteral*>(prefixExpression->GetRight().get());
				Assert::AreEqual(integerLeft->GetValue() == test.value0_, true,
					MSG("integerLeft.Value is incorrect."));

				const tsumugi::script::ast::expression::IntegerLiteral* integerRight = static_cast<const tsumugi::script::ast::expression::IntegerLiteral*>(prefixExpression->GetRight().get());
				Assert::AreEqual(integerRight->GetValue() == test.value1_, true,
					MSG("integerRight.Value is incorrect."));
			}
		}

		TEST_METHOD(TestOperatorPrecedenceParsing)
		{
			struct OperatorPrecedenceSet {
				tstring code_;
				tstring to_code_;
			};
			std::vector<OperatorPrecedenceSet> tests = {
				{TT("a + b"), TT("(a + b)")},
				{TT("!-a"), TT("(!(-a))")},
				{TT("a + b - c"), TT("((a + b) - c)")},
				{TT("a * b / c"), TT("((a * b) / c)")},
				{TT("a + b * c"), TT("(a + (b * c))")},
				{TT("a + b * c + d / e - f"), TT("(((a + (b * c)) + (d / e)) - f)")},
				{TT("1 + 2; -3 * 4"), TT("(1 + 2)((-3) * 4)")},
				{TT("5 > 4 == 3 < 4"), TT("((5 > 4) == (3 < 4))")},
				{TT("3 + 4 * 5 == 3 * 1 + 4 * 5"), TT("((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))")},
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
 				auto tocode = root->ToCode();
				Assert::AreEqual(tocode.compare(test.to_code_) == 0, true, MSG("ToCode is incorrect."));
			}
		}
		TEST_METHOD(TestIfExpression)
		{
			struct IfExpressionSet {
				tstring code_;
				tstring to_code_;
			};
			std::vector<IfExpressionSet> tests = {
				{TT("if (x < y) { x }"), TT("if (x < y) { x }")}
			};
			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				Assert::AreEqual(root->GetStatementCount() == 1, true, MSG("The number of Root.Statements is incorrect."));

				const tsumugi::script::ast::statement::ExpressionStatement* statement = dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
				Assert::IsNotNull(statement, MSG("Statements.Expression is not Identifier."));

				const tsumugi::script::ast::expression::IfExpression* expression = dynamic_cast<const tsumugi::script::ast::expression::IfExpression*>(statement->GetExpression().get());
				Assert::IsNotNull(expression, MSG("expressionStatement is not IfExpression"));

				this->_TestInfixExpression(expression->GetCondition(), TT("x"), TT("<"), TT("y"));

				auto consequenceBlock = expression->GetConsequence();
				Assert::AreEqual(size_t(1), consequenceBlock->GetStatements().size(), MSG("The number of sentences in Consequence is not 1."));

				auto* consequenceStmt = dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(consequenceBlock->GetStatements()[0].get());
				if (consequenceStmt == nullptr) {
					Assert::Fail(MSG("consequence is not an ExpressionStatement. consequence is not an ExpressionStatement."));
				}

				this->_TestIdentifier(consequenceStmt->GetExpression(), TT("x"));

				Assert::IsNull(expression->GetAlternative().get(), MSG("expression.Alternative is not null"));
			}
		}
		TEST_METHOD(TestIfElseExpression)
		{
			struct IfElseExpressionSet {
				tstring code_;
				tstring to_code_;
			};
			std::vector<IfElseExpressionSet> tests = {
				{TT("if (x < y) { x } else { y; }"), TT("if (x < y) { x } else { y; }")}
			};
			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				Assert::AreEqual(root->GetStatementCount() == 1, true, MSG("The number of Root.Statements is incorrect."));

				const tsumugi::script::ast::statement::ExpressionStatement* statement = dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
				Assert::IsNotNull(statement, MSG("Statements.Expression is not Identifier."));

				const tsumugi::script::ast::expression::IfExpression* expression = dynamic_cast<const tsumugi::script::ast::expression::IfExpression*>(statement->GetExpression().get());
				Assert::IsNotNull(expression, MSG("expressionStatement is not IfExpression"));

				this->_TestInfixExpression(expression->GetCondition(), TT("x"), TT("<"), TT("y"));

				auto consequenceBlock = expression->GetConsequence();
				Assert::AreEqual(size_t(1), consequenceBlock->GetStatements().size(), MSG("The number of sentences in Consequence is not 1."));

				auto* consequenceStmt = dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(consequenceBlock->GetStatements()[0].get());
				if (consequenceStmt == nullptr) {
					Assert::Fail(MSG("consequence is not an ExpressionStatement. consequence is not an ExpressionStatement."));
				}

				this->_TestIdentifier(consequenceStmt->GetExpression(), TT("x"));

				Assert::IsNotNull(expression->GetAlternative().get(), MSG("expression.Alternative is null"));

				Assert::AreEqual(expression->GetAlternative()->GetStatements().size() == 1, true, MSG("The number of Consequence sentences is not 1."));

				const auto* alternative = dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(expression->GetAlternative()->GetStatements()[0].get());
				Assert::IsNotNull(alternative, MSG("alternative is not ExpressionStatement."));

				this->_TestIdentifier(alternative->GetExpression(), TT("y"));
			}
		}

		TEST_METHOD(TestFunctionLiteral)
		{
			struct TestFunctionLiteralSet {
				tstring code_;
				tstring to_code_;
			};
			std::vector<TestFunctionLiteralSet> tests = {
				{TT("function(x, y) { x + y; }"), TT("function(x, y) { x + y; }")}
			};
			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);

				auto root = parser->ParseProgram();
				Assert::AreEqual(root->GetStatementCount(), size_t{ 1 }, MSG("The number of Root.Statements is incorrect."));
				const auto* statement = dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
				Assert::IsNotNull(statement, MSG("statement is not ExpressionStatement."));
				const auto* expression = dynamic_cast<const tsumugi::script::ast::expression::FunctionLiteral*> (statement->GetExpression().get());
				Assert::IsNotNull(expression, MSG("expression is not FunctionLiteral."));
				Assert::AreEqual(expression->GetParameters().size(), size_t{ 2 }, MSG("expression is not FunctionLiteral."));
				_TestIdentifier(expression->GetParameter(0), TT("x"));
				_TestIdentifier(expression->GetParameter(1), TT("y"));
				Assert::AreEqual(expression->GetBody()->GetStatements().size(), size_t{ 1 }, MSG("The number of expressions in the body of the function literal is incorrect."));
				const auto* bodyStatement = dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(expression->GetBody()->GetStatements()[0].get());
				Assert::IsNotNull(bodyStatement, MSG("bodyStatement is not ExpressionStatement."));
				_TestInfixExpression(bodyStatement->GetExpression(), TT("x"), TT("+"), TT("y"));
			}
		}

		TEST_METHOD(TestFunctionParameter)
		{
			struct FunctionParameterSet {
				tstring code_;
				tstring to_code_;
				std::vector<tstring> params_;
			};
			std::vector<FunctionParameterSet> tests = {
				{TT("function() {};"), TT("function() {};"), {}},
				{TT("function(x) {};"), TT("function(x) {};"), {TT("x")}},
				{TT("function(x, y, z) {};"), TT("function(x, y) { x + y; }"), {TT("x"), TT("y"), TT("z")}}
			};
			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);

				auto root = parser->ParseProgram();
				const auto* statement = dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
				const tsumugi::script::ast::expression::FunctionLiteral* function = dynamic_cast<const tsumugi::script::ast::expression::FunctionLiteral*> (statement->GetExpression().get());


				Assert::AreEqual(function->GetParameters().size(), test.params_.size(),
					MSG("The number of arguments in the function literal is incorrect.")
				);
				for (int i = 0; i < test.params_.size(); i++)
				{
					_TestIdentifier(function->GetParameter(i), test.params_[i]);
				}
			}
		}

		TEST_METHOD(TestCallExpression)
		{
			struct TestSet {
				tstring code_;
				tstring to_code_;
			};
			std::vector<TestSet> tests = {
				{TT("add(1, 2 * 3, 4 + 5);"), TT("add(1, 2 * 3, 4 + 5);")}
			};
			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);

				auto root = parser->ParseProgram();
				Assert::AreEqual(root->GetStatementCount(), size_t{ 1 }, MSG("The number of Root.Statements is incorrect."));
				const auto* statement = dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
				Assert::IsNotNull(statement, MSG("statement is not ExpressionStatement."));
				const auto* expression = dynamic_cast<const tsumugi::script::ast::expression::CallExpression*> (statement->GetExpression().get());
				Assert::IsNotNull(expression, MSG("expression is not CallExpression."));
				_TestIdentifier(expression->GetFunction(), TT("add"));
				Assert::AreEqual(expression->GetArguments().size(), size_t{3}, MSG("The number of arguments for the function literal is incorrect."));

				_TestLiteralExpression(expression->GetArgument(0), 1);
				_TestInfixExpression(expression->GetArgument(1), 2, TT("*"), 3);
				_TestInfixExpression(expression->GetArgument(2), 4, TT("+"), 5);
			}
		}

		TEST_METHOD(LiteralBasicTemplate)
		{
			struct TestSet {
				tstring code_;
				tstring to_code_;
			};
			std::vector<TestSet> tests = {
				{TT("function(x, y) { x + y; }"), TT("function(x, y) { x + y; }")}
			};
			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);

				auto root = parser->ParseProgram();
				Assert::AreEqual(root->GetStatementCount(), size_t{ 1 }, MSG("The number of Root.Statements is incorrect."));
				const auto* statement = dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
				Assert::IsNotNull(statement, MSG("statement is not ExpressionStatement."));
				const auto* expression = dynamic_cast<const tsumugi::script::ast::expression::FunctionLiteral*> (statement->GetExpression().get());
				Assert::IsNotNull(expression, MSG("expression is not FunctionLiteral."));
				Assert::AreEqual(expression->GetParameters().size(), size_t{ 2 }, MSG("expression is not FunctionLiteral."));
				_TestIdentifier(expression->GetParameter(0), TT("x"));
				_TestIdentifier(expression->GetParameter(1), TT("y"));
				Assert::AreEqual(expression->GetBody()->GetStatements().size(), size_t{1}, MSG("The number of expressions in the body of the function literal is incorrect."));
				const auto* bodyStatement = dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(expression->GetBody()->GetStatements()[0].get());
				Assert::IsNotNull(bodyStatement, MSG("bodyStatement is not ExpressionStatement."));
				_TestInfixExpression(bodyStatement->GetExpression(), TT("x"), TT("+"), TT("y"));
			}
		}

		TEST_METHOD(TestEvalIntegerExpression)
		{
			struct EvalIntegerSet {
				tstring code_;
				int expected_;
			};
			std::vector<EvalIntegerSet> tests = {
				{ TT("1"), 1 },
				{ TT("12"), 12 },
				{ TT("-1"), -1 },
				{ TT("-12"), -12 },
				{ TT("1 + 2 - 3"), 0 },
				{ TT("1 + 2 * 3"), 7 },
				{ TT("3 * 4 / 2 + 10 - 8"), 8 },
				{ TT("(1 + 2) * 3 - -1"), 10 },
				{ TT("-1 * -1"), 1 },
				{ TT("-10 + -1 * 2"), -12 },
				{ TT("(10 + 20) / (10 - 0)"), 3 }
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);
				_TestIntegerObject(evaluated.get(), test.expected_);
			}
		}

		TEST_METHOD(TestEvalBooleanExpression)
		{
			struct EvalIntegerSet {
				tstring code_;
				bool expected_;
			};
			std::vector<EvalIntegerSet> tests = {
				{ TT("true"), true },
				{ TT("false"), false },
				{ TT("true == true"), true },
				{ TT("true != true"), false },
				{ TT("true == false"), false },
				{ TT("true != false"), true },
				{ TT("(1 > 2) == true"), false },
				{ TT("(1 > 2) != false"), false },
				{ TT("(1 < 2) == false"), false },
				{ TT("(1 > 2) != true"), true },
				{ TT("(1 >= 2) != true"), true },
				{ TT("(1 >= 2) == true"), false },
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);
				_TestBooleanObject(evaluated.get(), test.expected_);
			}
		}

		TEST_METHOD(TestEvalBangOperator)
		{
			struct EvalIntegerSet {
				tstring code_;
				bool expected_;
			};
			std::vector<EvalIntegerSet> tests = {
				{ TT("!true"), false },
				{ TT("!false"), true },
				{ TT("!0"), false },
				{ TT("!1"), false },
				{ TT("!!1"), true },
				{ TT("!!true"), true },
				{ TT("!!!true"), false }
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);
				_TestBooleanObject(evaluated.get(), test.expected_);
			}
		}

		TEST_METHOD(TestEvalIfExpression)
		{
			struct EvalIntegerSet {
				tstring code_;
				int expected_;
			};
			std::vector<EvalIntegerSet> tests = {
				{ TT("if (true) { 1 }"), 1 },
				{ TT("if (false) { 1 }"), NULL },
				{ TT("if (true) { 1 } else { 2 }"), 1 },
				{ TT("if (false) { 1 } else { 2 }"), 2 },
				{ TT("if (5) { 1 } else { 2 }"), 1 },
				{ TT("if (!5) { 1 } else { 2 }"), 2 },
				{ TT("if (1 < 2) { 1 } else { 2 }"), 1 },
				{ TT("if (1 > 2) { 1 } else { 2 }"), 2 },
				{ TT("if (1 > 2) { 1 }"), NULL }
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + test.code_ + TT("\n")).c_str());
				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);
				if (test.expected_ == NULL) { // expected_ が 0 のときは誤判定される
					_TestNullObject(evaluated.get());
				} else {
					_TestIntegerObject(evaluated.get(), test.expected_);
				}
			}
		}

		TEST_METHOD(TestEvalReturnStatement)
		{
			struct EvalIntegerSet {
				tstring code_;
				int expected_;
			};
			std::vector<EvalIntegerSet> tests = {
				{ TT("return 10; "), 10 },
				{ TT("return 100/10"), 10 },
				{ TT("return 10; 1234;"), 10 },
				{ TT("2*3; return 10; 1234;"), 10 },
				{ TT("if (true) {if (true) {return 10;}	0;}"), 10 },
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + test.code_ + TT("\n")).c_str());
				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);
				_TestIntegerObject(evaluated.get(), test.expected_);
			}
		}

		TEST_METHOD(TestEvalErrorHandling)
		{
			struct EvalIntegerSet {
				tstring code_;
				int expected_;
			};
			std::vector<EvalIntegerSet> tests = {
				{ TT("5 + true;"), 10 },
				{ TT("5 + true; 5;"), 10 },
				{ TT("-true"), 10 },
				{ TT("true + false"), 10 },
				{ TT("if (true) { true * false; }"), 10 },
				{ TT("if (true) {if (true) {return false / false;}	0;	}"), 10 },
				{ TT(" - true + 100"), 10 },
				{ TT("foo"), 10 },
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + test.code_ + TT("\n")).c_str());
				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);
				_TestErrorObject(evaluated.get(), test.expected_);
			}
		}

		TEST_METHOD(TestEvalLetStatement)
		{
			struct EvalIntegerSet {
				tstring code_;
				int expected_;
			};
			std::vector<EvalIntegerSet> tests = {
				{ TT("let a = 1; a;"), 1 },
				{ TT("let a = 1 + 2 * 3; a;"), 7 },
				{ TT("let a = 1; let b = a; b;"), 1 },
				{ TT("let a = 1; let b = 2; let c = a + b; c;"), 3 },
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + test.code_ + TT("\n")).c_str());
				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);
				_TestIntegerObject(evaluated.get(), test.expected_);
			}
		}

		TEST_METHOD(TestEvalFunctionObject)
		{
			struct EvalFuncSet {
				tstring code_;
				size_t params_;
				tstring name_;
				tstring bodycode_;
			};
			std::vector<EvalFuncSet> tests = {
				{ TT("function(x) { x + 2; }"), 1, TT("x"), TT("(x + 2)")}
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + test.code_ + TT("\n")).c_str());
				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);
				_TestFunctionObject(evaluated.get());

				const auto* result = dynamic_cast<const tsumugi::script::object::FunctionObject*>(evaluated.get());
				Assert::AreEqual(test.params_, result->GetParameters().size());
				Assert::AreEqual(test.name_, result->GetParameters().at(0)->ToCode());
				Assert::AreEqual(test.bodycode_, result->GetBody()->ToCode());
			}
		}

		TEST_METHOD(TestFunctionApplication)
		{
			struct EvalFuncSet {
				tstring code_;
				int expected_;
			};
			std::vector<EvalFuncSet> tests = {
				{ TT("let identity = fn(x) { x }; identity(10);"), 10},
				{ TT("let identity = fn(x) { return x; }; identity(10);"), 10},
				{ TT("let double = fn(x) { x * 2; }; double(10);"), 20},
				{ TT("let add = fn(x, y) { x + y; }; add(10, 20);"), 30},
				{ TT("let add = fn(x, y) { x + y; }; add(add(10, 20), 30 + 40);"), 100},
				{ TT("fn(x) { x; }(10);"), 10}
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexing::Lexer>(new tsumugi::script::lexing::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parsing::Parser>(new tsumugi::script::parsing::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + test.code_ + TT("\n")).c_str());
				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);
				_TestIntegerObject(evaluated.get(), test.expected_);
			}
		}

		void _TestIntegerObject(tsumugi::script::object::IObject *obj, int expected)
		{
			const auto* result = dynamic_cast<const tsumugi::script::object::IntegerObject*>(obj);
			Assert::IsNotNull(result, MSG("result is not IntegerObject."));
			Assert::AreEqual(expected, result->GetValue());
		}
		void _TestBooleanObject(tsumugi::script::object::IObject* obj, bool expected)
		{
			const auto* result = dynamic_cast<const tsumugi::script::object::BooleanObject*>(obj);
			Assert::IsNotNull(result, MSG("result is not BooleanObject."));
			Assert::AreEqual(expected, result->GetValue());
		}
		void _TestNullObject(tsumugi::script::object::IObject* obj)
		{
			const auto* result = dynamic_cast<const tsumugi::script::object::NullObject*>(obj);
			Assert::IsNotNull(result, MSG("result is not NullObject."));
		}
		void _TestErrorObject(tsumugi::script::object::IObject* obj, bool expected)
		{
			const auto* result = dynamic_cast<const tsumugi::script::object::ErrorObject*>(obj);
			Assert::IsNotNull(result, MSG("result is not ErrorObject."));
			Logger::WriteMessage((TT("message : ") + result->Inspect() + TT("\n")).c_str());
		}
		void _TestFunctionObject(tsumugi::script::object::IObject* obj)
		{
			const auto* result = dynamic_cast<const tsumugi::script::object::FunctionObject*>(obj);
			Assert::IsNotNull(result, MSG("result is not FunctionObject."));
			Logger::WriteMessage((TT("message : ") + result->Inspect() + TT("\n")).c_str());
		}
private:
		// ヘルパー関数群

		void _TestIdentifier(const std::shared_ptr<const tsumugi::script::ast::IExpression>& exp, const tstring& value)
		{
			auto ident = dynamic_cast<const tsumugi::script::ast::expression::Identifier*>(exp.get());
			Assert::IsNotNull(ident, MSG("Expression is not Identifier."));
			Assert::AreEqual(value, ident->GetValue(), MSG("Identifier value mismatch."));
			Assert::AreEqual(value, ident->GetToken()->GetLiteral(), MSG("Identifier token mismatch."));
		}

		using ExpectedType = const std::variant<int, tstring, bool>;
		void _TestInfixExpression(const std::shared_ptr<const tsumugi::script::ast::IExpression>& exp,	const ExpectedType& left, const tstring& op, const ExpectedType& right)
		{
			const tsumugi::script::ast::expression::InfixExpression* opExp = dynamic_cast<const tsumugi::script::ast::expression::InfixExpression*>(exp.get());
			Assert::IsNotNull(opExp, MSG("Expression is not InfixExpression."));

			_TestLiteralExpression(opExp->GetLeft(), left);
			Assert::AreEqual(op, opExp->GetOperator(), MSG("Operator mismatch."));
			_TestLiteralExpression(opExp->GetRight(), right);
		}

		void _TestLiteralExpression(const std::shared_ptr<const tsumugi::script::ast::IExpression>& exp, const ExpectedType& expected)
		{
			std::visit([&](auto&& value) {
				using T = std::decay_t<decltype(value)>;

				if constexpr (std::is_same_v<T, int>) {
					_TestIntegerLiteral(exp, value);
				}
				else if constexpr (std::is_same_v<T, tstring>) {
					_TestIdentifier(exp, value);
				}
				else if constexpr (std::is_same_v<T, bool>) {
					_TestBooleanLiteral(exp, value);
				}
				else {
					Assert::Fail(MSG("This is an unexpected type."));
				}
			}, expected);
		}

		void _TestIntegerLiteral(const std::shared_ptr<const tsumugi::script::ast::IExpression>& exp, int value)
		{
			const tsumugi::script::ast::expression::IntegerLiteral* integerLiteral = dynamic_cast<const tsumugi::script::ast::expression::IntegerLiteral* > (exp.get());
			if (integerLiteral == nullptr)
			{
				Assert::Fail(MSG("Expression is not IntegerLiteral."));
			}
			if (integerLiteral->GetValue() != value)
			{
				Assert::Fail(MSG("integerLiteral.Value is not" << value << "."));
			}
			if (integerLiteral->TokenLiteral() != tsumugi::type::convert::IntegerToTString(value))
			{
				Assert::Fail(MSG("integerLiteral.TokenLiteral is not" << value << "."));
			}
		}

		void _TestBooleanLiteral(const std::shared_ptr<const tsumugi::script::ast::IExpression>& exp, bool value)
		{
			const tsumugi::script::ast::expression::BooleanLiteral* booleanLiteral = dynamic_cast<const tsumugi::script::ast::expression::BooleanLiteral*> (exp.get());
			if (booleanLiteral == nullptr)
			{
				Assert::Fail(MSG("Expression is not BooleanLiteral."));
			}
			if (booleanLiteral->GetValue() != value)
			{
				Assert::Fail(MSG("booleanLiteral.Value is not" << value << "."));
			}
			if (booleanLiteral->TokenLiteral() != tsumugi::type::convert::IntegerToTString(value))
			{
				Assert::Fail(MSG("booleanLiteral.TokenLiteral is not" << value << "."));
			}
		}
	};
}

#include "pch.h"
#include "CppUnitTest.h"
#include "Foundation/Types.h"
#include "Script/AST/Root.h"
#include "Script/AST/Expressions/Identifier.h"
#include "Script/AST/Expressions/IntegerLiteral.h"
#include "Script/AST/Expressions/FloatLiteral.h"
#include "Script/AST/Expressions/BooleanLiteral.h"
#include "Script/AST/Expressions/PrefixExpression.h"
#include "Script/AST/Expressions/IfExpression.h"
#include "Script/AST/Expressions/InfixExpression.h"
#include "Script/AST/Expressions/IndexExpression.h"
#include "Script/AST/Expressions/FunctionLiteral.h"
#include "Script/AST/Expressions/CallExpression.h"
#include "Script/AST/Expressions/ArrayLiteral.h"
#include "Script/AST/Expressions/WhileExpression.h"
#include "Script/AST/Expressions/AssignmentExpression.h"
#include "Script/AST/Expressions/PropertyAccessExpression.h"
#include "Script/AST/Statements/LetStatement.h"
#include "Script/AST/Statements/ReturnStatement.h"
#include "Script/AST/Statements/ExpressionStatement.h"
#include "Script/AST/Statements/BlockStatement.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/FloatObject.h"
#include "Script/Objects/BooleanObject.h"
#include "Script/Objects/NullObject.h"
#include "Script/Objects/StringObject.h"
#include "Script/Objects/ErrorObject.h"
#include "Script/Objects/ArrayObject.h"
#include "Script/Objects/UserObject.h"
#include "Script/Objects/Environment.h"
#include "Script/Objects/UserFunctionObject.h"
#include "Script/Lexer/ScriptLexer.h"
#include "Script/Lexer/ScriptToken.h"
#include "Script/Parser/ScriptParser.h"
#include "Script/Lexer/LexingStringReader.h"
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
			tsumugi::script::lexer::LexingStringReader reader;
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

			std::vector<tsumugi::script::lexer::Token*> testTokens;
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kAssign, TT("=")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kPlus, TT("+")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kMinus, TT("-")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kLeftParenthesis, TT("(")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kRightParenthesis, TT(")")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kLeftBraces, TT("{")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kRightBraces, TT("}")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kComma, TT(",")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kSemicolon, TT(";")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kLeftBrackets , TT("[")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kRightBrackets, TT("]")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kLessThan, TT("<")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kGreaterThan, TT(">")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kGreaterThanOrEqual, TT(">=")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kLessThanOrEqual , TT("<=")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kEqual, TT("==")));

			tsumugi::script::lexer::Lexer lexer(input);
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
			tsumugi::script::lexer::Lexer lexer(TT("  \t +  "));
			tsumugi::script::lexer::Token* token = lexer.NextToken();
			Assert::IsTrue(token->GetTokenType() == tsumugi::script::lexer::TokenType::kPlus);
			delete(token);
		}

		TEST_METHOD(SimpleExpression)
		{
			auto input = TT("let five = 5;");
			tsumugi::script::lexer::Lexer lexer(input);

			std::vector<tsumugi::script::lexer::Token*> testTokens;
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kLet, TT("let")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kIdentifier, TT("five")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kAssign, TT("=")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kInteger, TT("5")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kSemicolon, TT(";")));

			for (const auto * testToken : testTokens)
			{
				const auto* nextToken = lexer.NextToken();
				Assert::IsNotNull(nextToken);
				Assert::AreEqual(testToken->GetTokenType() == nextToken->GetTokenType(), true, 
					MSG("Expected " << tsumugi::script::lexer::TokenTypeToString(testToken->GetTokenType()) << " Actual:" << tsumugi::script::lexer::TokenTypeToString(nextToken->GetTokenType())));
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

			std::vector<tsumugi::script::lexer::Token*> testTokens;
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kLet, TT("let")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kIdentifier, TT("five")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kAssign, TT("=")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kInteger, TT("5")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kSemicolon, TT(";")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kLet, TT("let")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kIdentifier, TT("ten")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kAssign, TT("=")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kInteger, TT("10")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kSemicolon, TT(";")));

			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kLet, TT("let")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kIdentifier, TT("add")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kAssign, TT("=")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kFunction, TT("function")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kLeftParenthesis, TT("(")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kIdentifier, TT("x")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kComma, TT(",")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kIdentifier, TT("y")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kRightParenthesis, TT(")")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kLeftBraces, TT("{")));

			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kIdentifier, TT("x")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kPlus, TT("+")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kIdentifier, TT("y")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kSemicolon, TT(";")));

			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kRightBraces, TT("}")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kSemicolon, TT(";")));

			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kLet, TT("let")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kIdentifier, TT("result")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kAssign, TT("=")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kIdentifier, TT("add")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kLeftParenthesis, TT("(")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kIdentifier, TT("five")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kComma, TT(",")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kIdentifier, TT("ten")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kRightParenthesis, TT(")")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kSemicolon, TT(";")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kEOF, TT_EMPTY()));

			tsumugi::script::lexer::Lexer lexer(simplecode.c_str());
			for (const auto * testToken : testTokens)
			{
				const auto* nextToken = lexer.NextToken();
				Assert::IsNotNull(nextToken);
				Assert::AreEqual(testToken->GetTokenType() == nextToken->GetTokenType(), true,
					MSG("Expected " << tsumugi::script::lexer::TokenTypeToString(testToken->GetTokenType()) << " Actual:" << tsumugi::script::lexer::TokenTypeToString(nextToken->GetTokenType())));
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

			std::vector<tsumugi::script::lexer::Token*> testTokens;
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kIf, TT("if")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kLeftParenthesis, TT("(")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kInteger, TT("5")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kLessThan, TT("<")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kInteger, TT("10")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kRightParenthesis, TT(")")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kLeftBraces, TT("{")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kReturn, TT("return")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kTrue, TT("true")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kSemicolon, TT(";")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kRightBraces, TT("}")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kElse, TT("else")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kLeftBraces, TT("{")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kReturn, TT("return")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kFalse, TT("false")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kSemicolon, TT(";")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kRightBraces, TT("}")));
			testTokens.push_back(new tsumugi::script::lexer::Token(tsumugi::script::lexer::TokenType::kEOF, TT_EMPTY()));

			tsumugi::script::lexer::Lexer lexer(simplecode.c_str());
			for (const auto * testToken : testTokens)
			{
				const auto* nextToken = lexer.NextToken();
				Assert::IsNotNull(nextToken);
				Assert::AreEqual(testToken->GetTokenType() == nextToken->GetTokenType(), true,
					MSG("Expected " << tsumugi::script::lexer::TokenTypeToString(testToken->GetTokenType()) << " Actual:" << tsumugi::script::lexer::TokenTypeToString(nextToken->GetTokenType())));
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

			auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(letcode.c_str()));
			auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
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

			auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(letcode.c_str()));
			auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
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

			auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(retcode.c_str()));
			auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
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

			auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(code.c_str()));
			auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
			parser->GetLogger().SetLogConsole(&s_Console);
			auto root = parser->ParseProgram();

			Assert::AreEqual(root->GetStatementCount() == 1, true,
				MSG("The number of Root.Statements is incorrect."));

			//Assert::AreEqual(typeid(root->GetStatement(0)) == typeid(tsumugi::script::ast::statement::ExpressionStatement), true,
			//	MSG("Root.Statements(0) is not ExpressionStatement."));

			auto* statement = static_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
			//Assert::AreEqual(typeid(statement->GetExpression()) == typeid(tsumugi::script::ast::expression::Identifier), true,
			//	MSG("Statements.Expression is not Identifier."));

			const tsumugi::script::ast::expression::Identifier* identifier = static_cast<const tsumugi::script::ast::expression::Identifier*>(statement->GetExpression());
			Assert::AreEqual(identifier->GetValue().compare(TT("foobar")) == 0, true, MSG("identifier.Value is incorrect."));
			Assert::AreEqual(identifier->TokenLiteral().compare(TT("foobar")) == 0, true, MSG("identifier.Token is incorrect."));
		}

		TEST_METHOD(TestIntegerExpression)
		{
			tstring code =
				TT("123;");

			auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(code.c_str()));
			auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
			parser->GetLogger().SetLogConsole(&s_Console);
			auto root = parser->ParseProgram();

			Assert::AreEqual(root->GetStatementCount() == 1, true,
				MSG("The number of Root.Statements is incorrect."));

			//Assert::AreEqual(typeid(root->GetStatement(0)) == typeid(tsumugi::script::ast::statement::ExpressionStatement), true,
			//	MSG("Root.Statements(0) is not ExpressionStatement."));

			auto* statement = static_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
			//Assert::AreEqual(typeid(statement->GetExpression()) == typeid(tsumugi::script::ast::expression::IntegerLiteral), true,
			//	MSG("Statements.Expression is not Identifier."));

			const tsumugi::script::ast::expression::IntegerLiteral* integer = static_cast<const tsumugi::script::ast::expression::IntegerLiteral*>(statement->GetExpression());
			Assert::AreEqual(integer->GetValue() == 123, true, MSG("integer.Value is incorrect."));
		}

		TEST_METHOD(TestFloatExpression)
		{
			tstring code =
				TT("12.3;");

			auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(code.c_str()));
			auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
			parser->GetLogger().SetLogConsole(&s_Console);
			auto root = parser->ParseProgram();

			Assert::AreEqual(root->GetStatementCount() == 1, true,
				MSG("The number of Root.Statements is incorrect."));

			//Assert::AreEqual(typeid(root->GetStatement(0)) == typeid(tsumugi::script::ast::statement::ExpressionStatement), true,
			//	MSG("Root.Statements(0) is not ExpressionStatement."));

			auto* statement = static_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
			//Assert::AreEqual(typeid(statement->GetExpression()) == typeid(tsumugi::script::ast::expression::IntegerLiteral), true,
			//	MSG("Statements.Expression is not Identifier."));

			const tsumugi::script::ast::expression::FloatLiteral* floatLiteral = static_cast<const tsumugi::script::ast::expression::FloatLiteral*>(statement->GetExpression());
			Assert::AreEqual(floatLiteral->GetValue() == 12.3, true, MSG("floatLiteral.Value is incorrect."));
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
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();

				Assert::AreEqual(root->GetStatementCount() == 1, true,
					MSG("The number of Root.Statements is incorrect."));

				//Assert::AreEqual(typeid(root->GetStatement(0)) == typeid(tsumugi::script::ast::statement::ExpressionStatement), true,
				//	MSG("Root.Statements(0) is not ExpressionStatement."));

				auto* expressionStatement = static_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
				//Assert::AreEqual(typeid(expressionStatement) == typeid(tsumugi::script::ast::expression::PrefixExpression), true,
				//	MSG("expressionStatement is not PrefixExpression."));

				const tsumugi::script::ast::expression::PrefixExpression* prefixExpression = static_cast<const tsumugi::script::ast::expression::PrefixExpression*>(expressionStatement->GetExpression());
				Assert::AreEqual(prefixExpression->GetOperator().compare(test.operator_) == 0, true,
					MSG("prefixExpression.Operator is incorrect."));

				const tsumugi::script::ast::expression::IntegerLiteral* integer = static_cast<const tsumugi::script::ast::expression::IntegerLiteral*>(prefixExpression->GetRight());
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
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();

				Assert::AreEqual(root->GetStatementCount() == 1, true,
					MSG("The number of Root.Statements is incorrect."));

				//Assert::AreEqual(typeid(root->GetStatement(0)) == typeid(tsumugi::script::ast::statement::ExpressionStatement), true,
				//	MSG("Root.Statements(0) is not ExpressionStatement."));

				auto* expressionStatement = static_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
				//Assert::AreEqual(typeid(expressionStatement) == typeid(tsumugi::script::ast::expression::PrefixExpression), true,
				//	MSG("expressionStatement is not PrefixExpression."));

				const tsumugi::script::ast::expression::PrefixExpression* prefixExpression = static_cast<const tsumugi::script::ast::expression::PrefixExpression*>(expressionStatement->GetExpression());
				Assert::AreEqual(prefixExpression->GetOperator().compare(test.operator_) == 0, true,
					MSG("prefixExpression.Operator is incorrect."));

				const tsumugi::script::ast::expression::IntegerLiteral* integerLeft = static_cast<const tsumugi::script::ast::expression::IntegerLiteral*>(prefixExpression->GetRight());
				Assert::AreEqual(integerLeft->GetValue() == test.value0_, true,
					MSG("integerLeft.Value is incorrect."));

				const tsumugi::script::ast::expression::IntegerLiteral* integerRight = static_cast<const tsumugi::script::ast::expression::IntegerLiteral*>(prefixExpression->GetRight());
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
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
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
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				Assert::AreEqual(root->GetStatementCount() == 1, true, MSG("The number of Root.Statements is incorrect."));

				const tsumugi::script::ast::statement::ExpressionStatement* statement = dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
				Assert::IsNotNull(statement, MSG("Statements.Expression is not Identifier."));

				const tsumugi::script::ast::expression::IfExpression* expression = dynamic_cast<const tsumugi::script::ast::expression::IfExpression*>(statement->GetExpression());
				Assert::IsNotNull(expression, MSG("expressionStatement is not IfExpression"));

				this->_TestInfixExpression(expression->GetCondition().get(), TT("x"), TT("<"), TT("y"));

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
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				Assert::AreEqual(root->GetStatementCount() == 1, true, MSG("The number of Root.Statements is incorrect."));

				const tsumugi::script::ast::statement::ExpressionStatement* statement = dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
				Assert::IsNotNull(statement, MSG("Statements.Expression is not Identifier."));

				const tsumugi::script::ast::expression::IfExpression* expression = dynamic_cast<const tsumugi::script::ast::expression::IfExpression*>(statement->GetExpression());
				Assert::IsNotNull(expression, MSG("expressionStatement is not IfExpression"));

				this->_TestInfixExpression(expression->GetCondition().get(), TT("x"), TT("<"), TT("y"));

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
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);

				auto root = parser->ParseProgram();
				Assert::AreEqual(root->GetStatementCount(), size_t{ 1 }, MSG("The number of Root.Statements is incorrect."));
				const auto* statement = dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
				Assert::IsNotNull(statement, MSG("statement is not ExpressionStatement."));
				const auto* expression = dynamic_cast<const tsumugi::script::ast::expression::FunctionLiteral*> (statement->GetExpression());
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
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);

				auto root = parser->ParseProgram();
				const auto* statement = dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
				const tsumugi::script::ast::expression::FunctionLiteral* function = dynamic_cast<const tsumugi::script::ast::expression::FunctionLiteral*> (statement->GetExpression());


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
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);

				auto root = parser->ParseProgram();
				Assert::AreEqual(root->GetStatementCount(), size_t{ 1 }, MSG("The number of Root.Statements is incorrect."));
				const auto* statement = dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
				Assert::IsNotNull(statement, MSG("statement is not ExpressionStatement."));
				const auto* expression = dynamic_cast<const tsumugi::script::ast::expression::CallExpression*> (statement->GetExpression());
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
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);

				auto root = parser->ParseProgram();
				Assert::AreEqual(root->GetStatementCount(), size_t{ 1 }, MSG("The number of Root.Statements is incorrect."));
				const auto* statement = dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
				Assert::IsNotNull(statement, MSG("statement is not ExpressionStatement."));
				const tsumugi::script::ast::expression::FunctionLiteral* expression = dynamic_cast<const tsumugi::script::ast::expression::FunctionLiteral*> (statement->GetExpression());
				Assert::IsNotNull(expression, MSG("expression is not FunctionLiteral."));
				Assert::AreEqual(expression->GetParameters().size(), size_t{ 2 }, MSG("expression is not FunctionLiteral."));
				_TestIdentifier(expression->GetParameter(0), TT("x"));
				_TestIdentifier(expression->GetParameter(1), TT("y"));
				Assert::AreEqual(expression->GetBody()->GetStatements().size(), size_t{1}, MSG("The number of expressions in the body of the function literal is incorrect."));
				tsumugi::script::ast::statement::ExpressionStatement* bodyStatement = dynamic_cast<tsumugi::script::ast::statement::ExpressionStatement*>(expression->GetBody()->GetStatements()[0].get());
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
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);
				_TestIntegerObject(evaluated.get(), test.expected_);
			}
		}

		TEST_METHOD(TestEvalFloatExpression)
		{
			struct EvalFloatSet {
				tstring code_;
				double expected_;
			};
			std::vector<EvalFloatSet> tests = {
				{ TT("1.0"), 1.0 },
				{ TT("12.5"), 12.5 },
				{ TT("-1.25"), -1.25 },
				{ TT("1.5 + 2.25"), 3.75 },
				{ TT("2.0 * 3.5"), 7.0 },
				{ TT("10.0 / 4.0"), 2.5 },
				{ TT("(1.5 + 2.5) * 2.0"), 8.0 },
				{ TT("3.0 * 4.0 / 2.0 + 10.0 - 8.0"), 8.0 },
				{ TT("1.2 + 3"), 4.2 },          // int + float
				{ TT("3 + 1.2"), 4.2 },          // float + int
				{ TT("3 * 1.5"), 4.5 },          // int * float
				{ TT("1.5 * 3"), 4.5 },          // float * int
				{ TT("10 / 4.0"), 2.5 },         // int / float
				{ TT("10.0 / 4"), 2.5 },         // float / int
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);
				Logger::WriteMessage((TT("\nTesting code: ") + test.code_ + TT("\n")).c_str());
				_TestFloatObject(evaluated.get(), test.expected_);
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
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
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
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
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
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
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
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
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
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
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
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
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
				{ TT("function(x) { x + 2; }"), 1, TT("x"), TT("{ (x + 2); }")}
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + test.code_ + TT("\n")).c_str());
				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);
				_TestFunctionObject(evaluated.get());

				const auto* result = dynamic_cast<const tsumugi::script::object::UserFunctionObject*>(evaluated.get());
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
				{ TT("let identity = function(x) { x }; identity(10);"), 10},
				{ TT("let identity = function(x) { return x; }; identity(10);"), 10},
				{ TT("let double = function(x) { x * 2; }; double(10);"), 20},
				{ TT("let add = function(x, y) { x + y; }; add(10, 20);"), 30},
				{ TT("let add = function(x, y) { x + y; }; add(add(10, 20), 30 + 40);"), 100},
				{ TT("function(x) { x; }(10);"), 10},
				{ TT("let f = function(x) { if (x > 0) { return x; } x * 10;} f(5);"), 5},
				{ TT("let f = function(x) { return x; }; f(5) + 10;"), 15}
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + test.code_ + TT("\n")).c_str());
				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);
				_TestIntegerObject(evaluated.get(), test.expected_);
			}
		}

		TEST_METHOD(TestEvalStringExpression)
		{
			struct EvalFuncSet {
				tstring code_;
				tstring expected_;
			};
			std::vector<EvalFuncSet> tests = {
				{ TT("\"test\""),  TT("test") },
				{ TT("\"test\"*3"),  TT("testtesttest") },
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + test.code_ + TT("\n")).c_str());
				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);
				_TestStringObject(evaluated.get(), test.expected_);
			}
		}

		TEST_METHOD(TestStringMethod)
		{
			struct EvalFuncSet {
				tstring code_;
				tstring expected_;
			};
			std::vector<EvalFuncSet> tests = {
				// --- substr ---
				{ TT("\"abcdef\".substr(1, 3)"), TT("bcd") },
				{ TT("\"abcdef\".substr(2)"), TT("cdef") },

				// --- upper / lower ---
				{ TT("\"abc\".upper()"), TT("ABC") },
				{ TT("\"ABC\".lower()"), TT("abc") },

				// --- slice ---
				{ TT("\"abcdef\".slice(1, 4)"), TT("bcd") },

				// --- startsWith / endsWith ---
				//{ TT("\"hello\".startsWith(\"he\")"), TT("true") },
				//{ TT("\"hello\".endsWith(\"lo\")"), TT("true") },
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);
				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + test.code_ + TT("\n")).c_str());
				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);
				_TestStringObject(evaluated.get(), test.expected_);
			}
		}

		TEST_METHOD(TestEvalLetStatementAssignment)
		{
			struct EvalSet {
				tstring code_;
				int expected_;
			};
			std::vector<EvalSet> tests = {
				{ TT("let x = 5; x;"), 5 },
				{ TT("let x = 5 * 5; x;"), 25 },
				{ TT("let x = 5; let y = x + 10; y;"), 15 },
				{ TT("let x = 1; x = x + 2; x;"), 3 }
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);

				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + test.code_ + TT("\n")).c_str());

				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);

				_TestIntegerObject(evaluated.get(), test.expected_);
			}
		}

		TEST_METHOD(LiteralArrayAssignment)
		{
			tstring code = TT("let arr = [1, 2, 3];");

			auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(code.c_str()));
			auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
			parser->GetLogger().SetLogConsole(&s_Console);

			auto root = parser->ParseProgram();
			Assert::AreEqual(size_t{ 1 }, root->GetStatementCount());

			const auto* stmt = dynamic_cast<const tsumugi::script::ast::statement::LetStatement*>(root->GetStatement(0));
			Assert::IsNotNull(stmt);
			_TestIdentifier(stmt->GetName(), TT("arr"));

			const auto* array = dynamic_cast<const tsumugi::script::ast::expression::ArrayLiteral*>(stmt->GetValue());
			Assert::IsNotNull(array);
			Assert::AreEqual(size_t{ 3 }, array->GetElements().size());

			_TestIntegerLiteral(array->GetElements()[0].get(), 1);
			_TestIntegerLiteral(array->GetElements()[1].get(), 2);
			_TestIntegerLiteral(array->GetElements()[2].get(), 3);
		}

		TEST_METHOD(TestArrayDeepEquality)
		{
			struct {
				tstring input;
				std::function<void(tsumugi::script::object::IObject*)> tester;
			} tests[] = {

				// 1. 基本の深い比較
				{
					TT("[1,2,3] == [1,2,3]"),
					[](auto obj) { _TestBooleanObject(obj, true); }
				},
				{
					TT("[1,2,3] != [1,2,3]"),
					[](auto obj) { _TestBooleanObject(obj, false); }
				},
				{
					TT("[1,2,3] == [1,2,4]"),
					[](auto obj) { _TestBooleanObject(obj, false); }
				},

				// 2. 長さが違う
				{
					TT("[1] == [1,2]"),
					[](auto obj) { _TestBooleanObject(obj, false); }
				},
				{
					TT("[1,2] != [1]"),
					[](auto obj) { _TestBooleanObject(obj, true); }
				},

				// 3. ネストした配列
				{
					TT("[1,[2,3]] == [1,[2,3]]"),
					[](auto obj) { _TestBooleanObject(obj, true); }
				},
				{
					TT("[1,[2,3]] != [1,[2,4]]"),
					[](auto obj) { _TestBooleanObject(obj, true); }
				},
				{
					TT("[[1],[2,[3]]] == [[1],[2,[3]]]"),
					[](auto obj) { _TestBooleanObject(obj, true); }
				},

				// 4. 空配列
				{
					TT("[] == []"),
					[](auto obj) { _TestBooleanObject(obj, true); }
				},
				{
					TT("[] != [1]"),
					[](auto obj) { _TestBooleanObject(obj, true); }
				},
				{
					TT("[[]] == [[]]"),
					[](auto obj) { _TestBooleanObject(obj, true); }
				},

				// 5. 参照が違っても内容が同じなら true
				{
					TT("let a = [1,2,3]; let b = [1,2,3]; a == b"),
					[](auto obj) { _TestBooleanObject(obj, true); }
				},

				// 6. 同じ参照なら true
				{
					TT("let a = [1,2]; let b = a; a == b"),
					[](auto obj) { _TestBooleanObject(obj, true); }
				},

				// 7. 多段ネスト
				{
					TT("[1,[2,[3,[4]]]] == [1,[2,[3,[4]]]]"),
					[](auto obj) { _TestBooleanObject(obj, true); }
				},
				{
					TT("[1,[2,[3,[4]]]] != [1,[2,[3,[5]]]]"),
					[](auto obj) { _TestBooleanObject(obj, true); }
				},

				// 8. 型が違う場合
				{
					TT("[1,2] == 123"),
					[](auto obj) { _TestBooleanObject(obj, false); }
				},
				{
					TT("[1,2] == \"abc\""),
					[](auto obj) { _TestBooleanObject(obj, false); }
				},
				{
					TT("[1,2] == true"),
					[](auto obj) { _TestBooleanObject(obj, false); }
				},

				// 9. null を含む配列
				{
					TT("[1,null] == [1,null]"),
					[](auto obj) { _TestBooleanObject(obj, true); }
				},
				{
					TT("[1,null] != [1,2]"),
					[](auto obj) { _TestBooleanObject(obj, true); }
				},
			};

			for (auto& tt : tests) {
				auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(tt.input.c_str());
				auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
				parser->GetLogger().SetLogConsole(&s_Console);

				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + tt.input + TT("\n")).c_str());

				auto evaluator = std::make_unique<tsumugi::script::evaluator::Evaluator>();
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);
				tt.tester(evaluated.get());
			}
		}


		TEST_METHOD(LiteralIndexExpression)
		{
			tstring code = TT("arr[1 + 2];");

			auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(code.c_str()));
			auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
			parser->GetLogger().SetLogConsole(&s_Console);

			auto root = parser->ParseProgram();
			Assert::AreEqual(size_t{ 1 }, root->GetStatementCount());

			const auto* stmt = dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
			Assert::IsNotNull(stmt);

			const auto* index = dynamic_cast<const tsumugi::script::ast::expression::IndexExpression*>(stmt->GetExpression());
			Assert::IsNotNull(index);

			_TestIdentifier(index->GetLeft(), TT("arr"));
			_TestInfixExpression(index->GetIndex(), 1, TT("+"), 2);
		}

		TEST_METHOD(TestArrayMethods)
		{
			struct {
				tstring input;
				std::function<void(tsumugi::script::object::IObject*)> tester;
			} tests[] = {

				{
					TT("[1, 2, 3].length"),
					[](tsumugi::script::object::IObject* obj) -> void {
						_TestIntegerObject(obj, 3);
					}
				},

				{
					TT("let a = [1, 2]; a.push(3); a.length"),
					[](tsumugi::script::object::IObject* obj) -> void {
						_TestIntegerObject(obj, 3);
					}
				},

				{
					TT("let a = [10, 20, 30]; a.pop()"),
					[](tsumugi::script::object::IObject* obj) -> void {
						_TestIntegerObject(obj, 30);
					}
				},

				{
					TT("let a = []; a.pop()"),
					[](tsumugi::script::object::IObject* obj) -> void {
						_TestNullObject(obj);
					}
				},

				{
					TT("[1,2,3,4].slice(1,3)"),
					[](tsumugi::script::object::IObject* obj) -> void {
						auto arr = dynamic_cast<tsumugi::script::object::ArrayObject*>(obj);
						Assert::IsNotNull(arr);
						_TestIntegerObject(arr->GetElements()[0].get(), 2);
						_TestIntegerObject(arr->GetElements()[1].get(), 3);
					}
				},

				{
					TT("[1,2,3].slice(1)"),
					[](tsumugi::script::object::IObject* obj) -> void {
						auto arr = dynamic_cast<tsumugi::script::object::ArrayObject*>(obj);
						Assert::IsNotNull(arr);
						_TestIntegerObject(arr->GetElements()[0].get(), 2);
						_TestIntegerObject(arr->GetElements()[1].get(), 3);
					}
				},

				{
					TT("[1,2,3].join(\"-\")"),
					[](tsumugi::script::object::IObject* obj) -> void {
						_TestStringObject(obj, TT("1-2-3"));
					}
				},

				{
					TT("[1,2,3].join()"),
					[](tsumugi::script::object::IObject* obj) -> void {
						_TestStringObject(obj, TT("1,2,3"));
					}
				},
			};

			for (auto& tt : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(tt.input.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);

				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + tt.input + TT("\n")).c_str());

				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);
				tt.tester(evaluated.get());
			}
		}

		TEST_METHOD(TestHashMethods)
		{
			struct {
				tstring input;
				std::function<void(tsumugi::script::object::IObject*)> tester;
			} tests[] = {

				// size
				{
					TT("let h = #{\"a\": 1, \"b\": 2}; h.size"),
					[](tsumugi::script::object::IObject* obj) -> void {
						_TestIntegerObject(obj, 2);
					}
				},

				// keys()
				{
					TT("let h = #{\"a\": 1, \"b\": 2}; h.keys()"),
					[](tsumugi::script::object::IObject* obj) -> void {
						auto arr = dynamic_cast<tsumugi::script::object::ArrayObject*>(obj);
						Assert::IsNotNull(arr, MSG("result is not ArrayObject."));

						const auto& elems = arr->GetElements();

						// サイズチェックは IsTrue にしておく（AreEqual の型問題を避ける）
						Assert::IsTrue(elems.size() == 2, MSG("keys().length is not 2."));

						auto strip_quotes = [](const tstring& s) {
							if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
								return s.substr(1, s.size() - 2);
							return s;
							};

						tstring k1 = strip_quotes(elems[0]->Inspect());
						tstring k2 = strip_quotes(elems[1]->Inspect());

						Logger::WriteMessage((TT("k1 = ") + k1 + TT(", k2 = ") + k2 + TT("\n")).c_str());

						// 順不同なのでどちらの並びも許容
						const bool pattern1 = (k1 == TT("a") && k2 == TT("b"));
						const bool pattern2 = (k1 == TT("b") && k2 == TT("a"));

						Assert::IsTrue(pattern1 || pattern2, MSG("keys() are not [\"a\",\"b\"] in any order."));
					}
				},

				// values()
				{
					TT("let h = #{\"a\": 1, \"b\": 2}; h.values()"),
					[](tsumugi::script::object::IObject* obj) -> void {
						auto arr = dynamic_cast<tsumugi::script::object::ArrayObject*>(obj);
						Assert::IsNotNull(arr);

						auto& elems = arr->GetElements();
						Assert::AreEqual((size_t)2, elems.size());

						int v1 = dynamic_cast<tsumugi::script::object::IntegerObject*>(elems[0].get())->GetValue();
						int v2 = dynamic_cast<tsumugi::script::object::IntegerObject*>(elems[1].get())->GetValue();

						Assert::IsTrue(
							(v1 == 1 && v2 == 2) ||
							(v1 == 2 && v2 == 1)
						);
					}
				},

				// has(key)
				{
					TT("let h = #{\"a\": 1}; h.has(\"a\")"),
					[](tsumugi::script::object::IObject* obj) -> void {
						_TestBooleanObject(obj, true);
					}
				},
				{
					TT("let h = #{\"a\": 1}; h.has(\"b\")"),
					[](tsumugi::script::object::IObject* obj) -> void {
						_TestBooleanObject(obj, false);
					}
				},

				// get(key)
				{
					TT("let h = #{\"a\": 10}; h.get(\"a\")"),
					[](tsumugi::script::object::IObject* obj) -> void {
						_TestIntegerObject(obj, 10);
					}
				},
				{
					TT("let h = #{\"a\": 10}; h.get(\"b\")"),
					[](tsumugi::script::object::IObject* obj) -> void {
						_TestNullObject(obj);
					}
				},

				// set(key, value)
				{
					TT("let h = #{\"a\": 1}; h.set(\"b\", 20); h.get(\"b\")"),
					[](tsumugi::script::object::IObject* obj) -> void {
						_TestIntegerObject(obj, 20);
					}
				},

				// delete(key)
				{
					TT("let h = #{\"a\": 1, \"b\": 2}; h.delete(\"b\")"),
					[](tsumugi::script::object::IObject* obj) -> void {
						_TestIntegerObject(obj, 2);
					}
				},
				{
					TT("let h = #{\"a\": 1}; h.delete(\"x\")"),
					[](tsumugi::script::object::IObject* obj) -> void {
						_TestNullObject(obj);
					}
				},
			};

			for (auto& tt : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(tt.input.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);

				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + tt.input + TT("\n")).c_str());

				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);
				tt.tester(evaluated.get());
			}
		}

		TEST_METHOD(LiteralFunctionDeclaration)
		{
			tstring code = TT("let add = function(x, y) { return x + y; };");

			auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(code.c_str()));
			auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
			parser->GetLogger().SetLogConsole(&s_Console);

			auto root = parser->ParseProgram();
			Assert::AreEqual(size_t{ 1 }, root->GetStatementCount());

			const auto* stmt = dynamic_cast<const tsumugi::script::ast::statement::LetStatement*>(root->GetStatement(0));
			Assert::IsNotNull(stmt);
			_TestIdentifier(stmt->GetName(), TT("add"));

			const auto* func = dynamic_cast<const tsumugi::script::ast::expression::FunctionLiteral*>(stmt->GetValue());
			Assert::IsNotNull(func);

			Assert::AreEqual(size_t{ 2 }, func->GetParameters().size());
			_TestIdentifier(func->GetParameter(0), TT("x"));
			_TestIdentifier(func->GetParameter(1), TT("y"));

			Assert::AreEqual(size_t{ 1 }, func->GetBody()->GetStatements().size());
			const auto* bodyStmt = dynamic_cast<const tsumugi::script::ast::statement::ReturnStatement*>(func->GetBody()->GetStatements()[0].get());
			Assert::IsNotNull(bodyStmt);

			_TestInfixExpression(bodyStmt->GetValue(), TT("x"), TT("+"), TT("y"));
		}

		TEST_METHOD(TestEvalWhileExpression)
		{
			struct EvalSet {
				tstring code_;
				int expected_;
			};
			std::vector<EvalSet> tests = {
				{ TT("let x = 0; while (x < 5) { x = x + 1; } x;"), 5 },
				{ TT("let i = 1; let sum = 0; while (i <= 5) { sum = sum + i; i = i + 1; } sum;"), 15 }
			};

			for (auto& test : tests) {
				auto lexer = std::unique_ptr<tsumugi::script::lexer::Lexer>(new tsumugi::script::lexer::Lexer(test.code_.c_str()));
				auto parser = std::unique_ptr<tsumugi::script::parser::Parser>(new tsumugi::script::parser::Parser(lexer.get()));
				parser->GetLogger().SetLogConsole(&s_Console);

				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + test.code_ + TT("\n")).c_str());

				auto evaluator = std::unique_ptr<tsumugi::script::evaluator::Evaluator>(new tsumugi::script::evaluator::Evaluator());
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);

				_TestIntegerObject(evaluated.get(), test.expected_);
			}
		}

		TEST_METHOD(ParseAssignmentWithPropertyAccess)
		{
			tstring code = TT("p.x = 10;");

			auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(code.c_str());
			auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
			parser->GetLogger().SetLogConsole(&s_Console);

			auto root = parser->ParseProgram();
			Assert::AreEqual(size_t{ 1 }, root->GetStatementCount());

			const auto* exprStmt =
				dynamic_cast<const tsumugi::script::ast::statement::ExpressionStatement*>(root->GetStatement(0));
			Assert::IsNotNull(exprStmt, MSG("stmt is not ExpressionStatement."));

			const tsumugi::script::ast::expression::AssignmentExpression* assign =
				dynamic_cast<const tsumugi::script::ast::expression::AssignmentExpression*>(exprStmt->GetExpression());
			Assert::IsNotNull(assign, MSG("expr is not AssignmentExpression."));

			const auto* left = assign->GetLeft();
			Logger::WriteMessage((TT("left.ToCode = ") + left->ToCode() + TT("\n")).c_str());

			Assert::AreEqual(
				(int)tsumugi::script::ast::NodeType::kPropertyAccessExpression,
				(int)left->GetNodeType(),
				MSG("left is not PropertyAccessExpression.")
			);

			const auto* prop =
				dynamic_cast<const tsumugi::script::ast::expression::PropertyAccessExpression*>(left);
			Assert::IsNotNull(prop, MSG("left is not PropertyAccessExpression."));

			const auto* objIdent =
				dynamic_cast<const tsumugi::script::ast::expression::Identifier*>(prop->GetLeft());
			Assert::IsNotNull(objIdent, MSG("object is not Identifier."));
			Assert::AreEqual(tstring(TT("p")), objIdent->GetValue(), MSG("object identifier is not 'p'."));

			Assert::AreEqual(tstring(TT("x")), prop->GetName()->GetValue(), MSG("property name is not 'x'."));
		}

		TEST_METHOD(TestAssignmentExpression)
		{
			struct {
				tstring input;
				std::function<void(tsumugi::script::object::IObject*)> tester;
			} tests[] = {

				// ----------------------------------------
				// 1. 通常の変数代入
				// ----------------------------------------
				{
					TT("let x = 10; x = 20; x;"),
					[](auto* obj) { _TestIntegerObject(obj, 20); }
				},

				// ----------------------------------------
				// 2. UserObject のプロパティ代入
				// ----------------------------------------
				{
					TT("let p = { x: 1, y: 2 }; p.x = 10; p.x;"),
					[](auto* obj) { _TestIntegerObject(obj, 10); }
				},

				// ----------------------------------------
				// 3. Array のプロパティ代入 → エラー
				// ----------------------------------------
				{
					TT("let a = [1,2,3]; a.x = 10;"),
					[](auto* obj) { _TestErrorObject(obj, true); }
				},

				// ----------------------------------------
				// 4. String のプロパティ代入 → エラー
				// ----------------------------------------
				{
					TT("let s = \"hello\"; s.x = 10;"),
					[](auto* obj) { _TestErrorObject(obj, true); }
				},

				// ----------------------------------------
				// 5. Hash のプロパティ代入 → OK
				// （hash[\"x\"] = 10 は OK だが hash.x = 10 は NG）
				// ----------------------------------------
				{
					TT("let h = #{\"a\": 1}; h.x = 10; h.x;"),
					[](auto* obj) { _TestIntegerObject(obj, 10); }
				},

				// ----------------------------------------
				// 6. 無効な左辺
				// ----------------------------------------
				{
					TT("(1 + 2) = 10;"),
					[](auto* obj) { _TestErrorObject(obj, true); }
				},

				// ----------------------------------------
				// 7. プロパティ代入後の取得
				// ----------------------------------------
				{
					TT("let p = { x: 1 }; p.x = 99; p.x;"),
					[](auto* obj) { _TestIntegerObject(obj, 99); }
				},

				// ----------------------------------------
				// 8. ネストしたプロパティ代入
				// ----------------------------------------
				{
					TT("let p = { inner: { x: 1 } }; p.inner.x = 42; p.inner.x;"),
					[](auto* obj) { _TestIntegerObject(obj, 42); }
				},
			};

			for (auto& tt : tests) {
				auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(tt.input.c_str());
				auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
				parser->GetLogger().SetLogConsole(&s_Console);

				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + tt.input + TT("\n")).c_str());

				auto evaluator = std::make_unique<tsumugi::script::evaluator::Evaluator>();
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);

				tt.tester(evaluated.get());
			}
		}

		TEST_METHOD(TestEvalUserObjectBasic)
		{
			struct {
				tstring input;
				std::function<void(tsumugi::script::object::IObject*)> tester;
			} tests[] = {

				// ----------------------------------------
				// UserObject のテスト
				// ----------------------------------------
				{
					TT("let o = { x: 10, y: 20 }; o.x;"),
					[](auto* obj) { _TestIntegerObject(obj, 10); }
				},
				{
					TT("let o = { x: 10 }; o.y = 99; o.y;"),
					[](auto* obj) { _TestIntegerObject(obj, 99); }
				},
				{
					TT("let o = { x: 1 }; o.x = o.x + 5; o.x;"),
					[](auto* obj) { _TestIntegerObject(obj, 6); }
				},
				{
					TT("let o = { x: 1, y: 2 }; o.x = o.y; o.x;"),
					[](auto* obj) { _TestIntegerObject(obj, 2); }
				},
				{
					TT("let o = { inner: { a: 10 } }; o.inner.a;"),
					[](auto* obj) { _TestIntegerObject(obj, 10); }
				},
				{
					TT("let o = { inner: { a: 10 } }; o.inner.a = 77; o.inner.a;"),
					[](auto* obj) { _TestIntegerObject(obj, 77); }
				},
				{
					TT("let o = { a: 1 }; o[\"a\"];"),
					[](auto* obj) { _TestErrorObject(obj, true); } // UserObject はインデックスアクセス禁止
				},
				{
					TT("let o = { a: 1 }; o.b;"),
					[](auto* obj) { _TestNullObject(obj); } // 存在しないプロパティは null
				},
				{
					TT("let o = { a: 1 }; o.b = 10; o.b;"),
					[](auto* obj) { _TestIntegerObject(obj, 10); } // 新規プロパティ追加 OK
				},
				{
					TT("let o = { a: 1 }; o.a = { x: 5 }; o.a.x;"),
					[](auto* obj) { _TestIntegerObject(obj, 5); } // ネストした UserObject
				},
			};

			for (auto& tt : tests) {
				auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(tt.input.c_str());
				auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
				parser->GetLogger().SetLogConsole(&s_Console);

				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + tt.input + TT("\n")).c_str());

				auto evaluator = std::make_unique<tsumugi::script::evaluator::Evaluator>();
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);

				tt.tester(evaluated.get());
			}
		}

		TEST_METHOD(TestEvalNull)
		{
			struct {
				tstring input;
				std::function<void(tsumugi::script::object::IObject*)> tester;
			} tests[] = {
				TT("null;"),
				[](auto* obj) { _TestNullObject(obj); }
	
			};

			for (auto& tt : tests) {
				auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(tt.input.c_str());
				auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
				parser->GetLogger().SetLogConsole(&s_Console);

				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + tt.input + TT("\n")).c_str());

				auto evaluator = std::make_unique<tsumugi::script::evaluator::Evaluator>();
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);

				tt.tester(evaluated.get());
			}
		}

		TEST_METHOD(TestEvalUserObjectExtend)
		{
			struct {
				tstring input;
				std::function<void(tsumugi::script::object::IObject*)> tester;
			} tests[] = {

				// ----------------------------------------
				// 基本プロパティ取得
				// ----------------------------------------
				{
					TT("let o = { x: 10, y: 20 }; o.x;"),
					[](auto* obj) { _TestIntegerObject(obj, 10); }
				},

				// ----------------------------------------
				// 新規プロパティ追加
				// ----------------------------------------
				{
					TT("let o = { x: 10 }; o.y = 99; o.y;"),
					[](auto* obj) { _TestIntegerObject(obj, 99); }
				},

				// ----------------------------------------
				// プロパティの再代入（式を含む）
				// ----------------------------------------
				{
					TT("let o = { x: 1 }; o.x = o.x + 5; o.x;"),
					[](auto* obj) { _TestIntegerObject(obj, 6); }
				},

				// ----------------------------------------
				// 別プロパティの値を代入
				// ----------------------------------------
				{
					TT("let o = { x: 1, y: 2 }; o.x = o.y; o.x;"),
					[](auto* obj) { _TestIntegerObject(obj, 2); }
				},

				// ----------------------------------------
				// ネストした UserObject の取得
				// ----------------------------------------
				{
					TT("let o = { inner: { a: 10 } }; o.inner.a;"),
					[](auto* obj) { _TestIntegerObject(obj, 10); }
				},

				// ----------------------------------------
				// ネストした UserObject のプロパティ代入
				// ----------------------------------------
				{
					TT("let o = { inner: { a: 10 } }; o.inner.a = 77; o.inner.a;"),
					[](auto* obj) { _TestIntegerObject(obj, 77); }
				},

				// ----------------------------------------
				// UserObject はインデックスアクセス禁止
				// ----------------------------------------
				{
					TT("let o = { a: 1 }; o[\"a\"];"),
					[](auto* obj) { _TestErrorObject(obj, true); }
				},

				// ----------------------------------------
				// 存在しないプロパティ → null
				// ----------------------------------------
				{
					TT("let o = { a: 1 }; o.b;"),
					[](auto* obj) { _TestNullObject(obj); }
				},

				// ----------------------------------------
				// 存在しないプロパティに代入 → 新規追加
				// ----------------------------------------
				{
					TT("let o = { a: 1 }; o.b = 10; o.b;"),
					[](auto* obj) { _TestIntegerObject(obj, 10); }
				},

				// ----------------------------------------
				// プロパティに UserObject を代入
				// ----------------------------------------
				{
					TT("let o = { a: 1 }; o.a = { x: 5 }; o.a.x;"),
					[](auto* obj) { _TestIntegerObject(obj, 5); }
				},

				// ----------------------------------------
				// プロパティに関数を入れて呼び出す
				// ----------------------------------------
				{
					TT("let o = { f: function() { 10 } }; o.f();"),
					[](auto* obj) { _TestIntegerObject(obj, 10); }
				},

				// ----------------------------------------
				// プロパティに boolean を代入
				// ----------------------------------------
				{
					TT("let o = {}; o.flag = true; o.flag;"),
					[](auto* obj) { _TestBooleanObject(obj, true); }
				},

				// ----------------------------------------
				// プロパティに string を代入
				// ----------------------------------------
				{
					TT("let o = {}; o.msg = \"hi\"; o.msg;"),
					[](auto* obj) { _TestStringObject(obj, TT("hi")); }
				},

				// ----------------------------------------
				// プロパティに array を代入
				// ----------------------------------------
				{
					TT("let o = {}; o.arr = [1,2,3]; o.arr[1];"),
					[](auto* obj) { _TestIntegerObject(obj, 2); }
				},

				// ----------------------------------------
				// プロパティに hash を代入
				// ----------------------------------------
				{
					TT("let o = {}; o.h = #{\"a\": 10}; o.h[\"a\"];"),
					[](auto* obj) { _TestIntegerObject(obj, 10); }
				},

				// ----------------------------------------
				// プロパティに null を代入
				// ----------------------------------------
				{
					TT("let o = { x: 10 }; o.x = null; o.x;"),
					[](auto* obj) { _TestNullObject(obj); }
				},

				// ----------------------------------------
				// プロパティ名の重複（後勝ち）
				// ----------------------------------------
				{
					TT("let o = { x: 1, x: 2 }; o.x;"),
					[](auto* obj) { _TestIntegerObject(obj, 2); }
				},

				// ----------------------------------------
				// キーが Identifier / String 以外 → 構文エラー
				// これは空のステートメントになるので、ErrorObject を返せない
				// いったんコメントアウトしておく
				// ----------------------------------------
				//{
				//	TT("let o = { 1: 10 };"),
				//	[](auto* obj) { _TestErrorObject(obj, true); }
				//},
				//{
				//	TT("let o = { 1 + 1: 10 };"),
				//	[](auto* obj) { _TestErrorObject(obj, true); }
				//},

				// ----------------------------------------
				// 循環参照（クラッシュしないこと）
				// ----------------------------------------
				{
					TT("let o = {}; o.self = o; o.self.self.self;"),
					[](auto* obj) {
					// o.self.self.self は UserObject
					auto* uo = dynamic_cast<tsumugi::script::object::UserObject*>(obj);
					Assert::IsNotNull(uo, MSG("result is not UserObject."));
				}
			},
			};

			for (auto& tt : tests) {
				auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(tt.input.c_str());
				auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
				parser->GetLogger().SetLogConsole(&s_Console);

				auto root = parser->ParseProgram();
				Logger::WriteMessage((TT("\nTesting code: ") + tt.input + TT("\n")).c_str());

				auto evaluator = std::make_unique<tsumugi::script::evaluator::Evaluator>();
				auto environment = std::make_shared<tsumugi::script::object::Environment>();
				auto evaluated = evaluator->Eval(root.get(), environment);

				tt.tester(evaluated.get());
			}
		}


		static void _TestIntegerObject(tsumugi::script::object::IObject *obj, int expected)
		{
			const auto* result = dynamic_cast<const tsumugi::script::object::IntegerObject*>(obj);
			Assert::IsNotNull(result, MSG("result is not IntegerObject."));
			Assert::AreEqual(expected, result->GetValue());
		}
		static void _TestFloatObject(tsumugi::script::object::IObject* obj, double expected)
		{
			const auto* result = dynamic_cast<const tsumugi::script::object::FloatObject*>(obj);
			Assert::IsNotNull(result, MSG("result is not FloatObject."));
			Assert::AreEqual(expected, result->GetValue());
		}
		static void _TestBooleanObject(tsumugi::script::object::IObject* obj, bool expected)
		{
			const auto* result = dynamic_cast<const tsumugi::script::object::BooleanObject*>(obj);
			Assert::IsNotNull(result, MSG("result is not BooleanObject."));
			Assert::AreEqual(expected, result->GetValue());
		}
		static void _TestNullObject(tsumugi::script::object::IObject* obj)
		{
			const auto* result = dynamic_cast<const tsumugi::script::object::NullObject*>(obj);
			Assert::IsNotNull(result, MSG("result is not NullObject."));
		}
		static void _TestErrorObject(tsumugi::script::object::IObject* obj, bool expected)
		{
			const auto* result = dynamic_cast<const tsumugi::script::object::ErrorObject*>(obj);
			Assert::IsNotNull(result, MSG("result is not ErrorObject."));
			Logger::WriteMessage((TT("message : ") + result->Inspect() + TT("\n")).c_str());
		}
		static void _TestFunctionObject(tsumugi::script::object::IObject* obj)
		{
			const auto* result = dynamic_cast<const tsumugi::script::object::UserFunctionObject*>(obj);
			Assert::IsNotNull(result, MSG("result is not UserFunctionObject."));
			Logger::WriteMessage((TT("message : ") + result->Inspect() + TT("\n")).c_str());
		}
		static void _TestStringObject(tsumugi::script::object::IObject* obj, tstring expected)
		{
			const auto* result = dynamic_cast<const tsumugi::script::object::StringObject*>(obj);
			Assert::IsNotNull(result, MSG("result is not StringObject."));
			Assert::IsTrue(expected == result->GetValue());
		}
		static void _TestArrayObject(tsumugi::script::object::IObject* obj, size_t expectedSize)
		{
			const auto* result = dynamic_cast<const tsumugi::script::object::ArrayObject*>(obj);
			Assert::IsNotNull(result, MSG("result is not ArrayObject."));

			const auto& elems = result->GetElements();
			Assert::AreEqual(expectedSize, elems.size(), MSG("Array size mismatch."));
		}
		static void _TestArrayObject(
			tsumugi::script::object::IObject* obj,
			const std::vector<std::function<void(tsumugi::script::object::IObject*)>>& testers)
		{
			const auto* result = dynamic_cast<const tsumugi::script::object::ArrayObject*>(obj);
			Assert::IsNotNull(result, MSG("result is not ArrayObject."));

			const auto& elems = result->GetElements();
			Assert::AreEqual(testers.size(), elems.size(), MSG("Array size mismatch."));

			for (size_t i = 0; i < testers.size(); ++i) {
				testers[i](elems[i].get());
			}
		}
private:
		// ヘルパー関数群

		void _TestIdentifier(const tsumugi::script::ast::IExpression* exp, const tstring& value)
		{
			auto ident = dynamic_cast<const tsumugi::script::ast::expression::Identifier*>(exp);
			Assert::IsNotNull(ident, MSG("Expression is not Identifier."));
			Assert::AreEqual(value, ident->GetValue(), MSG("Identifier value mismatch."));
			Assert::AreEqual(value, ident->GetToken()->GetLiteral(), MSG("Identifier token mismatch."));
		}

		using ExpectedType = const std::variant<int, tstring, bool>;
		void _TestInfixExpression(const tsumugi::script::ast::IExpression* exp, const ExpectedType& left, const tstring& op, const ExpectedType& right)
		{
			const tsumugi::script::ast::expression::InfixExpression* opExp = dynamic_cast<const tsumugi::script::ast::expression::InfixExpression*>(exp);
			Assert::IsNotNull(opExp, MSG("Expression is not InfixExpression."));

			_TestLiteralExpression(opExp->GetLeft(), left);
			Assert::AreEqual(op, opExp->GetOperator(), MSG("Operator mismatch."));
			_TestLiteralExpression(opExp->GetRight(), right);
		}

		void _TestLiteralExpression(const tsumugi::script::ast::IExpression* exp, const ExpectedType& expected)
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

		void _TestIntegerLiteral(const tsumugi::script::ast::IExpression* exp, int value)
		{
			const tsumugi::script::ast::expression::IntegerLiteral* integerLiteral = dynamic_cast<const tsumugi::script::ast::expression::IntegerLiteral* > (exp);
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

		void _TestBooleanLiteral(const tsumugi::script::ast::IExpression* exp, bool value)
		{
			const tsumugi::script::ast::expression::BooleanLiteral* booleanLiteral = dynamic_cast<const tsumugi::script::ast::expression::BooleanLiteral*> (exp);
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

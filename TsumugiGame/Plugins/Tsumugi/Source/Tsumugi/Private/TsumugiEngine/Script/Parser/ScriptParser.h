#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Script/Lexer/ScriptTokenTypes.h"
#include "TsumugiEngine/Log/TextLogger.h"
#include "TsumugiEngine/Localization/Localizer.h"
#include <functional>

namespace tsumugi::script::lexer{ class Lexer; }
namespace tsumugi::script::lexer{ class Token; }
namespace tsumugi::script::ast { class Program; }
namespace tsumugi::script::ast { class IStatement; }
namespace tsumugi::script::ast { class IExpression; }
namespace tsumugi::script::ast::expression { class Identifier; }
namespace tsumugi::script::ast::statement { class LetStatement; }
namespace tsumugi::script::ast::statement { class ReturnStatement; }
namespace tsumugi::script::ast::statement { class ExpressionStatement; }
namespace tsumugi::script::ast::statement { class BlockStatement; }
namespace tsumugi::script::ast::statement { class FunctionStatement; }
namespace tsumugi::script::ast::statement { class ForStatement; }
namespace tsumugi::script::ast::statement { class BreakStatement; }
namespace tsumugi::script::ast::statement { class ContinueStatement; }
namespace tsumugi::script::ast::statement { class ClassStatement; }

namespace tsumugi::script::parser {

/// <summary>
/// 前置構文解析関数
/// </summary>
using PrefixParseFunction = std::function<std::unique_ptr<tsumugi::script::ast::IExpression> ()>;

/// <summary>
/// 中置構文解析関数
/// </summary>
using InfixParseFunction = std::function<std::unique_ptr<tsumugi::script::ast::IExpression> (std::unique_ptr<tsumugi::script::ast::IExpression>)>;

/// <summary>
/// 優先順位
/// </summary>
enum class Precedence {
    kLowest = 1,
    kAssign,      /// =
    kAndOr,       /// &&, ||
    kEquals,      /// ==
    kLessgreater, /// >, <, >=, <=
    kInstanceOf,  /// instanceOf
    kSum,         /// +
    kProduct,     /// *
    kPrefix,      /// -x, !x
    kCall,        /// myFunction(x)
};

class Parser {
public:
    Parser(tsumugi::script::lexer::Lexer* lexer);
    ~Parser();
    std::unique_ptr<ast::Program> ParseProgram();
    const auto& GetLexer() const { return lexer_; }
    log::TextLogger& GetLogger() { return logger_; }

public:
    static const std::unordered_map<tsumugi::script::lexer::TokenType, parser::Precedence> Precedences;

private:
    std::unique_ptr<script::ast::IStatement> ParseStatement();
    std::unique_ptr<script::ast::statement::LetStatement> ParseLetStatement();
    std::unique_ptr<script::ast::statement::ReturnStatement> ParseReturnStatement();
    std::unique_ptr<script::ast::statement::ExpressionStatement> ParseExpressionStatement();
    std::shared_ptr<script::ast::statement::BlockStatement> ParseBlockStatement();
    std::unique_ptr<script::ast::statement::FunctionStatement> ParseFunctionStatement();
    std::unique_ptr<script::ast::statement::ForStatement> ParseForStatement();
    std::unique_ptr<script::ast::statement::ClassStatement> ParseClassStatement();
    std::unique_ptr<script::ast::statement::BreakStatement> ParseBreakStatement();
    std::unique_ptr<script::ast::statement::ContinueStatement> ParseContinueStatement();
    std::unique_ptr<script::ast::IExpression> ParseExpression(Precedence precedence);
    std::unique_ptr<script::ast::IExpression> ParseIdentifier();
    std::unique_ptr<script::ast::IExpression> ParseIntegerLiteral();
    std::unique_ptr<script::ast::IExpression> ParseFloatLiteral();
    std::unique_ptr<script::ast::IExpression> ParseStringLiteral();
    std::unique_ptr<script::ast::IExpression> ParseBooleanLiteral();
    std::unique_ptr<script::ast::IExpression> ParseNullLiteral();
    std::unique_ptr<script::ast::IExpression> ParseSuperExpression();
    std::unique_ptr<script::ast::IExpression> ParseArrayLiteral();
    std::unique_ptr<script::ast::IExpression> ParseUserObjectLiteral();
    std::unique_ptr<script::ast::IExpression> ParseHashLiteral();
    std::unique_ptr<script::ast::IExpression> ParseGroupedExpression();
    std::unique_ptr<script::ast::IExpression> ParseIfExpression();
    std::unique_ptr<script::ast::IExpression> ParseWhileExpression();
    std::unique_ptr<script::ast::IExpression> ParseFunctionLiteral();
    std::unique_ptr<script::ast::IExpression> ParsePrefixExpression();
    std::unique_ptr<script::ast::IExpression> ParseInfixExpression(std::unique_ptr<script::ast::IExpression> left);
    std::unique_ptr<script::ast::IExpression> ParseCallExpression(std::unique_ptr<script::ast::IExpression> function);
    std::unique_ptr<script::ast::IExpression> ParseIndexExpression(std::unique_ptr<script::ast::IExpression> left);
    std::unique_ptr<script::ast::IExpression> ParseAssignmentExpression(std::unique_ptr<script::ast::IExpression> left);
    std::unique_ptr<script::ast::IExpression> ParsePropertyAccessExpression(std::unique_ptr<script::ast::IExpression> left);
    std::unique_ptr<script::ast::IExpression> ParseInstanceOfExpression(std::unique_ptr<script::ast::IExpression> left);
    bool ParseParameters(std::vector<std::shared_ptr<tsumugi::script::ast::expression::Identifier>>& parameters);
    bool ParseCallArguments(std::vector<std::unique_ptr<tsumugi::script::ast::IExpression>>& arguments);

    void ReadToken();
    bool CurrentTokenIs(const tsumugi::script::lexer::TokenType& type);
    bool PeekTokenIs(const tsumugi::script::lexer::TokenType& type);
    bool ExpectPeek(const tsumugi::script::lexer::TokenType& type);
    const auto& GetCurrentToken() const { return currentToken_; }
    const auto& GetNextToken() const { return nextToken_; }
    const Precedence GetCurrentPrecedence() const;
    const Precedence GetNextPrecedence() const;
    bool ExpectPeekRequiredTokenType(const tsumugi::script::lexer::TokenType tokenType, const std::string& symbol);

    void RegisterPrefixParseFunctions();
    void RegisterInfixParseFunctions();

private:
    log::TextLogger logger_;
    i18n::Localizer localizer;
    std::shared_ptr<tsumugi::script::lexer::Token> currentToken_;
    std::shared_ptr<tsumugi::script::lexer::Token> nextToken_;
    tsumugi::script::lexer::Lexer* lexer_;
    std::unordered_map<tsumugi::script::lexer::TokenType, PrefixParseFunction> prefixParseFunctions_;
    std::unordered_map<tsumugi::script::lexer::TokenType, InfixParseFunction> infixParseFunctions_;
};

}

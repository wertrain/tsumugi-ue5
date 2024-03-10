#pragma once

#include "Foundation/Types.h"
#include "Script/Lexing/ScriptTokenTypes.h"
#include "Log/TextLogger.h"
#include "Localization/Localizer.h"
#include <functional>

namespace tsumugi::script::lexing { class Lexer; }
namespace tsumugi::script::lexing { class Token; }
namespace tsumugi::script::ast { class Root; }
namespace tsumugi::script::ast { class IStatement; }
namespace tsumugi::script::ast { class IExpression; }
namespace tsumugi::script::ast::expression { class Identifier; }
namespace tsumugi::script::ast::statement { class LetStatement; }
namespace tsumugi::script::ast::statement { class ReturnStatement; }
namespace tsumugi::script::ast::statement { class ExpressionStatement; }
namespace tsumugi::script::ast::statement { class BlockStatement; }

namespace tsumugi::script::parsing {

/// <summary>
/// 前置構文解析関数
/// </summary>
using PrefixParseFunction = std::function<tsumugi::script::ast::IExpression* ()>;

/// <summary>
/// 中置構文解析関数
/// </summary>
using InfixParseFunction = std::function<tsumugi::script::ast::IExpression* (tsumugi::script::ast::IExpression*)>;

/// <summary>
/// 優先順位
/// </summary>
enum class Precedence {
    kLowest = 1,
    kAssign,      /// =
    kAndOr,       /// &&, ||
    kEquals,      /// ==
    kLessgreater, /// >, <, >=, <=
    kSum,         /// +
    kProduct,     /// *
    kPrefix,      /// -x, !x
    kCall,        /// myFunction(x)
};

class Parser {
public:
    Parser(tsumugi::script::lexing::Lexer* lexer);
    virtual ~Parser();
    void ReadToken();
    script::ast::Root* ParseRoot();
    script::ast::IStatement* ParseStatement();
    script::ast::statement::LetStatement* ParseLetStatement();
    script::ast::statement::ReturnStatement* ParseReturnStatement();
    script::ast::statement::ExpressionStatement* ParseExpressionStatement();
    script::ast::statement::BlockStatement* ParseBlockStatement();
    script::ast::IExpression* ParseExpression(Precedence precedence);
    script::ast::IExpression* ParseIdentifier();
    script::ast::IExpression* ParseIntegerLiteral();
    script::ast::IExpression* ParseBooleanLiteral();
    script::ast::IExpression* ParseGroupedExpression();
    script::ast::IExpression* ParseIfExpression();
    script::ast::IExpression* ParseFunctionLiteral();
    script::ast::IExpression* ParsePrefixExpression();
    script::ast::IExpression* ParseInfixExpression(const script::ast::IExpression* left);
    script::ast::IExpression* ParseCallExpression(const script::ast::IExpression* function);
    bool ParseParameters(std::vector<std::shared_ptr<tsumugi::script::ast::expression::Identifier>>& parameters);
    bool ParseCallArguments(std::vector<std::shared_ptr<tsumugi::script::ast::IExpression>>& arguments);
    script::ast::Root* ParseProgram();

    bool ExpectPeek(const tsumugi::script::lexing::TokenType& type);
    const auto& GetCurrentToken() const { return currentToken_; }
    const auto& GetNextToken() const { return nextToken_; }
    const Precedence GetCurrentPrecedence() const;
    const Precedence GetNextPrecedence() const;
    const auto& GetLexer() const { return lexer_; }
    log::TextLogger& GetLogger() { return logger_; }

public:
    static const std::unordered_map<tsumugi::script::lexing::TokenType, Precedence> Precedences;

private:
    void RegisterPrefixParseFunctions();
    void RegisterInfixParseFunctions();
    bool ExpectPeekRequiredTokenType(const tsumugi::script::lexing::TokenType tokenType, const std::string& symbol);

private:
    log::TextLogger logger_;
    i18n::Localizer localizer;
    std::shared_ptr<tsumugi::script::lexing::Token> currentToken_;
    std::shared_ptr<tsumugi::script::lexing::Token> nextToken_;
    tsumugi::script::lexing::Lexer* lexer_;
    std::unordered_map<tsumugi::script::lexing::TokenType, PrefixParseFunction> prefixParseFunctions_;
    std::unordered_map<tsumugi::script::lexing::TokenType, InfixParseFunction> infixParseFunctions_;
};

}

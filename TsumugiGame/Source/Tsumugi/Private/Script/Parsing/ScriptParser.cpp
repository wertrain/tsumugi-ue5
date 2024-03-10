#include "Script/Parsing/ScriptParser.h"
#include "Script/Lexing/ScriptLexer.h"
#include "Script/Lexing/ScriptToken.h"
#include "Script/AbstractSyntaxTree/Root.h"
#include "Script/AbstractSyntaxTree/Statements/LetStatement.h"
#include "Script/AbstractSyntaxTree/Statements/ReturnStatement.h"
#include "Script/AbstractSyntaxTree/Statements/ExpressionStatement.h"
#include "Script/AbstractSyntaxTree/Statements/BlockStatement.h"
#include "Script/AbstractSyntaxTree/Expressions/Identifier.h"
#include "Script/AbstractSyntaxTree/Expressions/IntegerLiteral.h"
#include "Script/AbstractSyntaxTree/Expressions/BooleanLiteral.h"
#include "Script/AbstractSyntaxTree/Expressions/IfExpression.h"
#include "Script/AbstractSyntaxTree/Expressions/FunctionLiteral.h"
#include "Script/AbstractSyntaxTree/Expressions/PrefixExpression.h"
#include "Script/AbstractSyntaxTree/Expressions/InfixExpression.h"
#include "Script/AbstractSyntaxTree/Expressions/CallExpression.h"
#include <cassert>
#include <unordered_map>

namespace tsumugi::script::parsing {

const std::unordered_map<tsumugi::script::lexing::TokenType, Precedence> Parser::Precedences = {
    { lexing::TokenType::kEqual, Precedence::kEquals },
    { lexing::TokenType::kNotEqual, Precedence::kEquals },
    { lexing::TokenType::kLessThan, Precedence::kLessgreater },
    { lexing::TokenType::kGreaterThan, Precedence::kLessgreater },
    { lexing::TokenType::kPlus, Precedence::kSum },
    { lexing::TokenType::kMinus, Precedence::kSum },
    { lexing::TokenType::kSlash, Precedence::kProduct },
    { lexing::TokenType::kAsterisk, Precedence::kProduct },
    { lexing::TokenType::kLeftParenthesis, Precedence::kCall },
};

Parser::Parser(lexing::Lexer* lexer)
 : logger_()
 , localizer()
 , currentToken_()
 , nextToken_()
 , lexer_(lexer) {

    assert(lexer != nullptr);

    // 2つ分のトークンを読み込んでセットしておく
    currentToken_ = std::shared_ptr<lexing::Token>(lexer_->NextToken());
    nextToken_ = std::shared_ptr<lexing::Token>(lexer_->NextToken());

    RegisterPrefixParseFunctions();
    RegisterInfixParseFunctions();
}

Parser::~Parser() {

}

void Parser::ReadToken() {

    currentToken_ = std::move(nextToken_);
    nextToken_.reset();
    nextToken_ = std::unique_ptr<lexing::Token>(lexer_->NextToken());
}

ast::Root* Parser::ParseRoot() {

    return nullptr;
}

ast::IStatement* Parser::ParseStatement() {

    switch (currentToken_.get()->GetTokenType()) {
    case lexing::TokenType::kLet:
        return ParseLetStatement();
    case lexing::TokenType::kReturn :
        return ParseReturnStatement();
    default:
        return ParseExpressionStatement();
    }
}

ast::statement::LetStatement* Parser::ParseLetStatement() {

    auto statement = std::make_unique<ast::statement::LetStatement>();
    statement->SetToken(currentToken_);

    if (!ExpectPeekRequiredTokenType(lexing::TokenType::kIdentifier, "Identifier")) {
        return nullptr;
    }

    auto name = std::make_shared<ast::expression::Identifier>(currentToken_, currentToken_->GetLiteral());
    statement->SetName(name);

    // 次は = (等号)がある
    if (!ExpectPeekRequiredTokenType(lexing::TokenType::kAssign, "=")) {
        return nullptr;
    }
    // = を読み飛ばす
    ReadToken();

    auto expression = std::shared_ptr<ast::IExpression>(ParseExpression(Precedence::kLowest));
    statement->SetValue(expression);

    // セミコロンは必須ではない
    if (nextToken_->GetTokenType() == lexing::TokenType::kSemicolon) {
        ReadToken();
    }

    return statement.release();
}

ast::statement::ReturnStatement* Parser::ParseReturnStatement() {

    auto statement = std::make_unique<ast::statement::ReturnStatement> ();
    statement->SetToken(currentToken_);

    ReadToken();

    auto expression = std::shared_ptr<ast::IExpression>(ParseExpression(Precedence::kLowest));
    statement->SetValue(expression);

    // セミコロンは必須ではない
    if (nextToken_->GetTokenType() == lexing::TokenType::kSemicolon) {
        ReadToken();
    }

    return statement.release();
}

ast::statement::ExpressionStatement* Parser::ParseExpressionStatement() {

    auto statement = std::make_unique<ast::statement::ExpressionStatement>();
    statement->SetToken(currentToken_);

    auto expression = std::shared_ptr<ast::IExpression>(ParseExpression(Precedence::kLowest));
    statement->SetExpression(expression);

    // セミコロンは必須ではない
    if (nextToken_->GetTokenType() == lexing::TokenType::kSemicolon) {
        ReadToken();
    }

    return statement.release();
}

script::ast::statement::BlockStatement* Parser::ParseBlockStatement() {

    auto block = std::make_unique<ast::statement::BlockStatement>(currentToken_);

    // "{" を読み飛ばす
    ReadToken();

    while (
        currentToken_->GetTokenType() != lexing::TokenType::kRightBraces &&
        currentToken_->GetTokenType() != lexing::TokenType::kEOF) {

        auto statement = std::shared_ptr<tsumugi::script::ast::IStatement>(ParseStatement());
        if (statement) {
            block->AddStatement(statement);
        }
        ReadToken();
    }

    return block.release();
}

ast::IExpression* Parser::ParseExpression(const Precedence precedence) {

    ast::IExpression* left = nullptr;
    if (auto it = prefixParseFunctions_.find(currentToken_->GetTokenType()); it != prefixParseFunctions_.end()) {
        auto [keyFound, value] = *it;
        left = value();
    }
    else {
        std::unordered_map<std::string, std::string> placeholders = {
            {"0", lexing::TokenTypeToString(currentToken_->GetTokenType())},
        };
        logger_.Log(log::TextLogger::Categories::Error, localizer.GetMessage(i18n::MessageId::kPrefixParseFunctionMissing, placeholders));
        return nullptr;
    }

    while (nextToken_->GetTokenType() != lexing::TokenType::kSemicolon && precedence < GetNextPrecedence()) {

        if (auto it = infixParseFunctions_.find(nextToken_->GetTokenType()); it != infixParseFunctions_.end()) {
            auto [keyFound, value] = *it;
            ReadToken(); 
            left = value(left);
        }
        else {
            return left;
        }
    }

    return left;
}

ast::IExpression* Parser::ParseIdentifier() {

    return std::make_unique<ast::expression::Identifier>(currentToken_, currentToken_->GetLiteral()).release();
}

script::ast::IExpression* Parser::ParseIntegerLiteral() {

    int result = 0;
    if (type::convert::FromChars(currentToken_->GetLiteral(), result)) {
        return std::make_unique<ast::expression::IntegerLiteral>(currentToken_, result).release();
    } else {
        std::unordered_map<std::string, tstring> placeholders = {
            {"0", currentToken_->GetLiteral()},
            {"1", tstring(TT("Integer"))}
        };
        logger_.Log(log::TextLogger::Categories::Error, localizer.GetMessage(i18n::MessageId::kConversionFailed, placeholders));
        return nullptr;
    }
}

script::ast::IExpression* Parser::ParseBooleanLiteral() {

    return std::make_unique<ast::expression::BooleanLiteral>(currentToken_, currentToken_->GetTokenType() == lexing::TokenType::kTrue).release();
}

script::ast::IExpression* Parser::ParseGroupedExpression() {

    // "(" を読み飛ばす
    ReadToken();

    // 括弧内の式を解析する
    auto* expression = ParseExpression(Precedence::kLowest);

    // 閉じ括弧 ")" がないとエラー
    if (!ExpectPeekRequiredTokenType(lexing::TokenType::kRightParenthesis, ")")) {
        return nullptr;
    }

    return expression;
}

script::ast::IExpression* Parser::ParseIfExpression() {

    auto expression = std::make_unique<ast::expression::IfExpression>(currentToken_);

    if (!ExpectPeekRequiredTokenType(lexing::TokenType::kLeftParenthesis, "(")) {
        return nullptr;
    }

    ReadToken();

    auto condition = std::shared_ptr<tsumugi::script::ast::IExpression>(ParseExpression(Precedence::kLowest));
    expression->SetCondition(condition);

    if (!ExpectPeekRequiredTokenType(lexing::TokenType::kRightParenthesis, ")")) {
        return nullptr;
    }
    if (!ExpectPeekRequiredTokenType(lexing::TokenType::kLeftBraces, "{")) {
        return nullptr;
    }

    // ブロック文の解析
    auto blockif = std::shared_ptr<tsumugi::script::ast::statement::BlockStatement>(ParseBlockStatement());
    expression->SetConsequence(blockif);

    if (nextToken_->GetTokenType() == lexing::TokenType::kElse) {
        
        ReadToken();

        if (!ExpectPeekRequiredTokenType(lexing::TokenType::kLeftBraces, "{")) {
            return nullptr;
        }

        auto blockelse = std::shared_ptr<tsumugi::script::ast::statement::BlockStatement>(ParseBlockStatement());
        expression->SetAlternative(blockelse);
    }

    return expression.release();
}

script::ast::IExpression* Parser::ParseFunctionLiteral() {

    auto expression = std::make_unique<ast::expression::FunctionLiteral>(currentToken_);

    if (!ExpectPeekRequiredTokenType(lexing::TokenType::kLeftParenthesis, "(")) {
        return nullptr;
    }

    std::vector<std::shared_ptr<tsumugi::script::ast::expression::Identifier>> parameters;
    if (!ParseParameters(parameters)) {
        return nullptr;
    }
    for (auto& parameter : parameters) {
        expression->AddParameter(parameter);
    }

    if (!ExpectPeekRequiredTokenType(lexing::TokenType::kLeftBraces, "{")) {
        return nullptr;
    }

    auto block = std::shared_ptr<tsumugi::script::ast::statement::BlockStatement>(ParseBlockStatement());
    expression->SetBody(block);

    return expression.release();
}

script::ast::IExpression* Parser::ParsePrefixExpression() {

    auto expression = std::make_unique<ast::expression::PrefixExpression>(currentToken_, currentToken_->GetLiteral());

    ReadToken();

    expression->SetRight(std::shared_ptr<const tsumugi::script::ast::IExpression>(ParseExpression(Precedence::kPrefix)));

    return expression.release();
}

script::ast::IExpression* Parser::ParseInfixExpression(const script::ast::IExpression* left) {

    auto expression = std::make_unique<ast::expression::InfixExpression>(currentToken_, currentToken_->GetLiteral(), std::shared_ptr<const script::ast::IExpression>(left));

    const auto currentPrecedence = GetCurrentPrecedence();
    ReadToken();
    expression->SetRight(std::shared_ptr<const tsumugi::script::ast::IExpression>(ParseExpression(currentPrecedence)));

    return expression.release();
}

script::ast::IExpression* Parser::ParseCallExpression(const script::ast::IExpression* function) {

    auto expression = std::make_unique<ast::expression::CallExpression>(currentToken_);
    expression->SetFunction(std::shared_ptr<const script::ast::IExpression>(function));
    
    std::vector<std::shared_ptr<tsumugi::script::ast::IExpression>> arguments;
    if (!ParseCallArguments(arguments)) {
        return nullptr;
    }
    for (auto& argument : arguments) {
        expression->AddArgument(argument);
    }

    return expression.release();
}

bool Parser::ParseParameters(std::vector<std::shared_ptr<tsumugi::script::ast::expression::Identifier>>& parameters) {

    parameters.clear();

    // 引数なしの関数の場合
    if (nextToken_->GetTokenType() == lexing::TokenType::kRightParenthesis) {
        ReadToken(); 
        return true;
    }
    ReadToken();

    auto first = std::make_shared<ast::expression::Identifier>(currentToken_, currentToken_->GetLiteral());
    parameters.push_back(first);

    while (nextToken_->GetTokenType() == lexing::TokenType::kComma) {

        // カンマか読み込み済み識別子をスキップ
        ReadToken();
        ReadToken();

        auto param = std::make_shared<ast::expression::Identifier>(currentToken_, currentToken_->GetLiteral());
        parameters.push_back(param);
    }

    if (!ExpectPeekRequiredTokenType(lexing::TokenType::kRightParenthesis, ")")) {
        parameters.clear(); 
        return false;
    }

    return true;
}

bool Parser::ParseCallArguments(std::vector<std::shared_ptr<tsumugi::script::ast::IExpression>>& arguments) {

    arguments.clear();

    // ( を読み飛ばす
    ReadToken();

    // 引数なしの関数呼び出しの場合
    if (currentToken_->GetTokenType() == lexing::TokenType::kRightParenthesis) {
        return true;
    }

    // 一つ目の引数を解析
    auto* first_argument = ParseExpression(parsing::Precedence::kLowest);
    if (first_argument == nullptr) {
        return false;
    }
    arguments.push_back(std::shared_ptr<ast::IExpression>(first_argument));

    // 2つ目以降の引数があればそれを解析
    while (nextToken_->GetTokenType() == lexing::TokenType::kComma) {

        // カンマか読み込み済み識別子をスキップ
        ReadToken();
        ReadToken();

        auto* argument = ParseExpression(parsing::Precedence::kLowest);
        if (argument == nullptr) {
            return false;
        }
        arguments.push_back(std::shared_ptr<ast::IExpression>(argument));
    }

    if (!ExpectPeekRequiredTokenType(lexing::TokenType::kRightParenthesis, ")")) {
        arguments.clear();
        return false;
    }

    return true;
}

ast::Root* Parser::ParseProgram() {

    auto root = std::make_unique<ast::Root>();
    while (currentToken_.get()->GetTokenType() != lexing::TokenType::kEOF) {
        if (auto* statement = ParseStatement()) {
            root->AddStatement(statement);
        }
        ReadToken();
    }
    return root.release();
}

bool Parser::ExpectPeek(const lexing::TokenType& type) {

    if (nextToken_->GetTokenType() == type) {
        ReadToken();
        return true;
    }
    std::unordered_map<std::string, std::string> placeholders = {
        {"0", lexing::TokenTypeToString(nextToken_->GetTokenType())},
        {"1", lexing::TokenTypeToString(type)}
    };
    logger_.Log(log::TextLogger::Categories::Error, localizer.GetMessage(i18n::MessageId::kInvalidInputFormat, placeholders));
    return false;
}

const Precedence Parser::GetCurrentPrecedence() const {

    if (auto it = Precedences.find(currentToken_->GetTokenType()); it != Precedences.end()) {
        auto [keyFound, value] = *it;
        return value;
    }
    else {
        return Precedence::kLowest;
    }
}

const Precedence Parser::GetNextPrecedence() const {

    if (auto it = Precedences.find(nextToken_->GetTokenType()); it != Precedences.end()) {
        auto [keyFound, value] = *it;
        return value;
    }
    else {
        return Precedence::kLowest;
    }
}

void Parser::RegisterPrefixParseFunctions() {

    prefixParseFunctions_.emplace(lexing::TokenType::kIdentifier, [this] { return ParseIdentifier(); });
    prefixParseFunctions_.emplace(lexing::TokenType::kInteger, [this] { return ParseIntegerLiteral(); });
    prefixParseFunctions_.emplace(lexing::TokenType::kBang, [this] { return ParsePrefixExpression(); });
    prefixParseFunctions_.emplace(lexing::TokenType::kMinus, [this] { return ParsePrefixExpression(); });
    prefixParseFunctions_.emplace(lexing::TokenType::kTrue, [this] { return ParseBooleanLiteral(); });
    prefixParseFunctions_.emplace(lexing::TokenType::kFalse, [this] { return ParseBooleanLiteral(); });
    prefixParseFunctions_.emplace(lexing::TokenType::kLeftParenthesis, [this] { return ParseGroupedExpression(); });
    prefixParseFunctions_.emplace(lexing::TokenType::kIf, [this] { return ParseIfExpression(); });
    prefixParseFunctions_.emplace(lexing::TokenType::kFunction, [this] { return ParseFunctionLiteral(); });
}

void Parser::RegisterInfixParseFunctions() {

    infixParseFunctions_.emplace(lexing::TokenType::kPlus, [this](auto left) { return ParseInfixExpression(left); });
    infixParseFunctions_.emplace(lexing::TokenType::kMinus, [this](auto left) { return ParseInfixExpression(left); });
    infixParseFunctions_.emplace(lexing::TokenType::kSlash, [this](auto left) { return ParseInfixExpression(left); });
    infixParseFunctions_.emplace(lexing::TokenType::kAsterisk, [this](auto left) { return ParseInfixExpression(left); });
    infixParseFunctions_.emplace(lexing::TokenType::kEqual, [this](auto left) { return ParseInfixExpression(left); });
    infixParseFunctions_.emplace(lexing::TokenType::kNotEqual, [this](auto left) { return ParseInfixExpression(left); });
    infixParseFunctions_.emplace(lexing::TokenType::kLessThan, [this](auto left) { return ParseInfixExpression(left); });
    infixParseFunctions_.emplace(lexing::TokenType::kGreaterThan, [this](auto left) { return ParseInfixExpression(left); });
    infixParseFunctions_.emplace(lexing::TokenType::kLeftParenthesis, [this](auto left) { return ParseCallExpression(left); });
}

bool Parser::ExpectPeekRequiredTokenType(const tsumugi::script::lexing::TokenType tokenType, const std::string& symbol) {

    if (!ExpectPeek(tokenType)) {
        std::unordered_map<std::string, std::string> placeholders = {
            {"0", symbol}
        };
        logger_.Log(log::TextLogger::Categories::Error, localizer.GetMessage(i18n::MessageId::kRequiredSymbolMissing, placeholders));
        return false;
    }
    return true;
}

}

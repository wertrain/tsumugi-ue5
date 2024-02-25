#include "Script/Parsing/ScriptParser.h"
#include "Script/Lexing/ScriptLexer.h"
#include "Script/Lexing/ScriptToken.h"
#include "Script/AbstractSyntaxTree/Root.h"
#include "Script/AbstractSyntaxTree/Expressions/Identifier.h"
#include "Script/AbstractSyntaxTree/Statements/LetStatement.h"
#include "Script/AbstractSyntaxTree/Statements/ReturnStatement.h"
#include "Script/AbstractSyntaxTree/Statements/ExpressionStatement.h"
#include "Script/AbstractSyntaxTree/Expressions/IntegerLiteral.h"
#include <cassert>
#include <unordered_map>

namespace tsumugi::script::parsing {

Parser::Parser(lexing::Lexer* lexer)
    : logger_()
    , localizer()
    , currentToken_()
    , nextToken_()
    , lexer_(lexer) {

    assert(lexer != nullptr);

    // 2つ分のトークンを読み込んでセットしておく
    currentToken_ = std::unique_ptr<lexing::Token>(lexer_->NextToken());
    nextToken_ = std::unique_ptr<lexing::Token>(lexer_->NextToken());

    RegisterPrefixParseFunctions();
    RegisterPrefixParseFunctions();
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

    if (false == ExpectPeek(lexing::TokenType::kIdentifier)) {
        return nullptr;
    }

    auto name = std::make_shared<ast::expression::Identifier>(currentToken_, currentToken_->GetLiteral());
    statement->SetName(name);

    // 次は = (等号)がある
    if (false == ExpectPeek(lexing::TokenType::kAssign)) {
        return nullptr;
    }

    // = を読み飛ばす
    ReadToken();

    // 式(let文の右辺)
    // TODO: 後で実装。
    while (currentToken_->GetTokenType() != lexing::TokenType::kSemicolon) {
        ReadToken();
    }

    return statement.release();
}

ast::statement::ReturnStatement* Parser::ParseReturnStatement() {

    auto statement = std::make_unique<ast::statement::ReturnStatement> ();
    statement->SetToken(currentToken_);

    // 式(return文の右辺)
    // TODO: 後で実装。
    while (currentToken_->GetTokenType() != lexing::TokenType::kSemicolon) {
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

ast::IExpression* Parser::ParseExpression(const Precedence precedence) {

    if (auto it = prefixParseFunctions_.find(currentToken_->GetTokenType()); it != prefixParseFunctions_.end()) {
        auto [keyFound, value] = *it;
        return value();
    }
    else {
        return nullptr;
    }
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

void Parser::RegisterPrefixParseFunctions() {

    prefixParseFunctions_.insert({lexing::TokenType::kIdentifier, std::bind(&Parser::ParseIdentifier, this)});
    prefixParseFunctions_.insert({lexing::TokenType::kInteger, std::bind(&Parser::ParseIntegerLiteral, this)});
}

void Parser::RegisterInfixParseFunctions() {

}


}

#include "Text/Parser/TextParser.h"
#include "Text/Lexer/TextLexer.h"
#include "Text/Lexer/TextToken.h"
#include "Text/AST/ITextStatement.h"
#include "Text/AST/TextProgram.h"
#include "Text/AST/Statements/LabelStatement.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/AST/Statements/TextStatement.h"
#include <cassert>

namespace tsumugi::text::parser {

Parser::Parser(lexer::Lexer* lexer)
    : errors_()
    , currentToken_()
    , nextToken_()
    , lexer_(lexer) {

    assert(lexer != nullptr);

    // 2つ分のトークンを読み込んでセットしておく
    currentToken_ = std::shared_ptr<lexer::Token>(lexer_->NextToken());
    nextToken_ = std::shared_ptr<lexer::Token>(lexer_->NextToken());
}

Parser::~Parser() = default;

std::unique_ptr<ast::Program> Parser::ParseProgram() {

    auto root = std::make_unique<ast::Program>();
    while (currentToken_.get()->GetTokenType() != lexer::TokenType::kEOF) {
        if (auto statement = ParseStatement()) {
            root->AddStatement(std::move(statement));
        }
        ReadToken();
    }
    return root;
}

std::unique_ptr<ast::IStatement> Parser::ParseStatement() {

    if (currentToken_->IsLineHead()) {
        switch (currentToken_.get()->GetTokenType()) {
            case lexer::TokenType::kAsterisk:
                return ParseLabelStatement();
            case lexer::TokenType::kAtMark:
                return ParseAtMarkTagStatement();
        }
    }
    switch (currentToken_.get()->GetTokenType()) {
        case lexer::TokenType::kTagOpen:
            return ParseTagStatement();
    }
    return ParseTextStatement();
}

std::unique_ptr<ast::statement::LabelStatement> Parser::ParseLabelStatement() {

    auto statement = std::make_unique<ast::statement::LabelStatement>(std::move(currentToken_));

    // 次のトークンへ（ラベル名）
    ReadToken();

    statement->SetLabelName(currentToken_->GetLiteral());

    // パイプがあれば見出し付き
    if (!PeekTokenIs(lexer::TokenType::kPipe)) {
        return statement;
    }

    // パイプを読み飛ばし
    ReadToken();

    // 次のトークンへ（見出し名）
    ReadToken();

    statement->SetLabelHeadline(currentToken_->GetLiteral());

    return statement;
}

std::unique_ptr<text::ast::statement::TagStatement> Parser::ParseTagStatement() {

    auto statement = std::make_unique<ast::statement::TagStatement>(std::move(currentToken_));

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kString, "tag")) {
        return nullptr;
    }

    statement->SetTagName(currentToken_->GetLiteral());

    // 属性なしか判定
    if (PeekTokenIs(lexer::TokenType::kTagClose)) {
        return statement;
    }

    while (!PeekTokenIs(lexer::TokenType::kTagClose)) {
        if (!ExpectPeekRequiredTokenType(lexer::TokenType::kString, "attribute name")) {
            return nullptr;
        }
        tstring key = currentToken_->GetLiteral();
        if (!ExpectPeekRequiredTokenType(lexer::TokenType::kAssign, "=")) {
            return nullptr;
        }
        if (!ExpectPeekRequiredTokenType(lexer::TokenType::kString, "attribute value")) {
            return nullptr;
        }
        tstring value = currentToken_->GetLiteral();
        statement->AddAttribute(key, value);
    }
    ReadToken();
    return statement;
}

std::unique_ptr<text::ast::statement::TagStatement> Parser::ParseAtMarkTagStatement() {

    auto statement = std::make_unique<ast::statement::TagStatement>(std::move(currentToken_));

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kString, "tag")) {
        return nullptr;
    }
    statement->SetTagName(currentToken_->GetLiteral());

    if (PeekTokenIs(lexer::TokenType::kTagClose)) {
        return statement;
    }
    while (!PeekTokenIs(lexer::TokenType::kNewLine) && !PeekTokenIs(lexer::TokenType::kEOF)) {
        if (!ExpectPeekRequiredTokenType(lexer::TokenType::kString, "attribute name")) {
            return nullptr;
        }
        tstring key = currentToken_->GetLiteral();
        if (!ExpectPeekRequiredTokenType(lexer::TokenType::kAssign, "=")) {
            return nullptr;
        }
        if (!ExpectPeekRequiredTokenType(lexer::TokenType::kString, "attribute value")) {
            return nullptr;
        }
        tstring value = currentToken_->GetLiteral();
        statement->AddAttribute(key, value);
    }
    ReadToken();
    return statement;
}

std::unique_ptr<text::ast::statement::TextStatement> Parser::ParseTextStatement() {

    auto statement = std::make_unique<ast::statement::TextStatement>(std::move(currentToken_));
    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kString, "tag")) {
        return nullptr;
    }
    statement->SetText(currentToken_->GetLiteral());
    return statement;
}

void Parser::ReadToken() {

    currentToken_ = nextToken_;
    nextToken_.reset();
    nextToken_ = std::shared_ptr<lexer::Token>(lexer_->NextToken());
}

bool Parser::CurrentTokenIs(const lexer::TokenType& type) {

    if (currentToken_->GetTokenType() == type) {
        return true;
    }
    return false;
}

bool Parser::PeekTokenIs(const lexer::TokenType& type) {

    if (nextToken_->GetTokenType() == type) {
        return true;
    }
    return false;
}

bool Parser::ExpectPeek(const lexer::TokenType& type) {

    if (nextToken_->GetTokenType() == type) {
        ReadToken();
        return true;
    }
    errors_.LogError(i18n::MessageId::kInvalidInputFormat, lexer::TokenTypeToString(nextToken_->GetTokenType()), lexer::TokenTypeToString(type));
    return false;
}

bool Parser::ExpectPeekRequiredTokenType(const lexer::TokenType tokenType, const std::string& symbol) {

    if (!ExpectPeek(tokenType)) {
        errors_.LogError(i18n::MessageId::kRequiredSymbolMissing, symbol);
        return false;
    }
    return true;
}

}

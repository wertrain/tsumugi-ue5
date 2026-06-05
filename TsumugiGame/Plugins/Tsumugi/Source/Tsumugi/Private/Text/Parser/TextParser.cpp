#include "Text/Parser/TextParser.h"
#include "Text/Lexer/TextLexer.h"
#include "Text/Lexer/TextToken.h"
#include "Text/AST/ITextStatement.h"
#include "Text/AST/TextProgram.h"
#include "Text/AST/Statements/LabelStatement.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/AST/Statements/TextStatement.h"
#include "Text/AST/Statements/ScriptBlockStatement.h"
#include <cassert>

namespace tsumugi::text::parser {

struct BlockTagDefinition {
    tstring begin;
    tstring end;
};

static const BlockTagDefinition kScriptBlock {
    TT("iscript"),
    TT("endscript")
};

static const BlockTagDefinition kMacroBlock {
    TT("macro"),
    TT("endmacro")
};

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
            case lexer::TokenType::kSemiColon:
                return ParseCommentStatement();
        }
    }
    switch (currentToken_.get()->GetTokenType()) {
        case lexer::TokenType::kTagOpen:
            return ParseTagStatement();
        case lexer::TokenType::kNewLine:
            return nullptr;
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

std::unique_ptr<text::ast::IStatement> Parser::ParseTagStatementCommon(std::function<bool()> shouldContinue) {

    auto statement = std::make_unique<ast::statement::TagStatement>(std::move(currentToken_));

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kString, "tag")) {
        return nullptr;
    }

    statement->SetTagName(currentToken_->GetLiteral());

    // 属性なしか判定
    if (PeekTokenIs(lexer::TokenType::kTagClose)) {
        // iscript タグがチェックする
        if (statement->GetTagName() == kScriptBlock.begin) {
            tstring rawScript;
            if (!lexer_->ReadRawUntil(rawScript, TT("[") + kScriptBlock.end + TT("]"))) {
                errors_.LogError(i18n::MessageId::kUnclosedBlock, kScriptBlock.begin, TT("[") + kScriptBlock.end + TT("]"));
                return nullptr;
            }
            // ReadRawUntil を呼び出した後はトークンの同期が必要
            SyncTokens();

            auto scriptStatement = std::make_unique<ast::statement::ScriptBlockStatement>(std::move(statement->GetTokenShared()));
            scriptStatement->SetScriptText(rawScript);
            return scriptStatement;
        }

        // 改行があれば進める
        ReadToken();
        if (PeekTokenIs(lexer::TokenType::kNewLine)) {
            ReadToken();
        }
        return statement;
    }

    while (shouldContinue()) {
        // 属性=値 を判定
        // それ以外のタイプが来れば失敗
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

    // 改行があれば進める
    if (PeekTokenIs(lexer::TokenType::kNewLine)) {
        ReadToken();
    }

    return statement;
}

std::unique_ptr<text::ast::IStatement> Parser::ParseTagStatement() {

    return ParseTagStatementCommon([this]() { return !PeekTokenIs(lexer::TokenType::kTagClose); });
}

std::unique_ptr<text::ast::IStatement> Parser::ParseAtMarkTagStatement() {

    return ParseTagStatementCommon([this]() {
        return !PeekTokenIs(lexer::TokenType::kNewLine) && !PeekTokenIs(lexer::TokenType::kEOF);
    });
}

std::unique_ptr<text::ast::statement::TextStatement> Parser::ParseTextStatement() {

    tstring text = currentToken_->GetLiteral();
    auto statement = std::make_unique<ast::statement::TextStatement>(std::move(currentToken_));
    statement->SetText(text);
    return statement;
}

std::unique_ptr<text::ast::IStatement> Parser::ParseCommentStatement() {

    while (!CurrentTokenIs(lexer::TokenType::kNewLine) && !CurrentTokenIs(lexer::TokenType::kEOF)) {
        ReadToken();
    }
    // 常に nullptr を返しておく
    return nullptr;
}

void Parser::ReadToken() {

    currentToken_ = nextToken_;
    nextToken_.reset();
    nextToken_ = std::shared_ptr<lexer::Token>(lexer_->NextToken());
}

void Parser::SyncTokens() {

    currentToken_ = std::shared_ptr<lexer::Token>(lexer_->NextToken());
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

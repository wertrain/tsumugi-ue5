#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Text/Lexer/TextTokenTypes.h"
#include "TsumugiEngine/Common/ErrorReporter.h"
#include <functional>

namespace tsumugi::text::lexer { class Lexer; }
namespace tsumugi::text::lexer { class Token; }
namespace tsumugi::text::ast { class Program; }
namespace tsumugi::text::ast { class IStatement; }
namespace tsumugi::text::ast::statement { class LabelStatement; }
namespace tsumugi::text::ast::statement { class TagStatement; }
namespace tsumugi::text::ast::statement { class TextStatement; }

namespace tsumugi::text::parser {

class Parser {public:
    Parser(tsumugi::text::lexer::Lexer* lexer);
    ~Parser();

    std::unique_ptr<ast::Program> ParseProgram();


    const auto& GetLexer() const { return lexer_; }
    //log::TextLogger& GetLogger() { return reporter_.GetLogger(); }

private:
    std::unique_ptr<text::ast::IStatement> ParseStatement();
    std::unique_ptr<text::ast::statement::LabelStatement> ParseLabelStatement();
    std::unique_ptr<text::ast::IStatement> ParseTagStatementCommon(std::function<bool()> shouldContinue);
    std::unique_ptr<text::ast::IStatement> ParseTagStatement();
    std::unique_ptr<text::ast::IStatement> ParseAtMarkTagStatement();
    std::unique_ptr<text::ast::statement::TextStatement> ParseTextStatement();
    std::unique_ptr<text::ast::IStatement> ParseCommentStatement();

    void ReadToken();
    void SyncTokens();
    bool CurrentTokenIs(const text::lexer::TokenType& type);
    bool PeekTokenIs(const text::lexer::TokenType& type);
    bool ExpectPeek(const text::lexer::TokenType& type);
    bool ExpectPeekRequiredTokenType(const text::lexer::TokenType tokenType, const std::string& symbol);
    const auto& GetCurrentToken() const { return currentToken_; }
    const auto& GetNextToken() const { return nextToken_; }

private:
    common::ErrorReporter errors_;
    std::shared_ptr<text::lexer::Token> currentToken_;
    std::shared_ptr<text::lexer::Token> nextToken_;
    text::lexer::Lexer* lexer_;
};

}

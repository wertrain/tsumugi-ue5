#pragma once

#include "Foundation/Types.h"
#include "Text/Lexer/TextTokenTypes.h"
#include "Common/ErrorReporter.h"

namespace tsumugi::text::lexer { class Lexer; }
namespace tsumugi::text::lexer { class Token; }
namespace tsumugi::text::ast { class Program; }
namespace tsumugi::text::ast { class IStatement; }
namespace tsumugi::text::ast::statement { class LabelStatement; }

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

    void ReadToken();
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

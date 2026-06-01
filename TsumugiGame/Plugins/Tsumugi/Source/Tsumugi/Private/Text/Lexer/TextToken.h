#pragma once

#include "Foundation/Types.h"
#include "Text/Lexer/TextTokenTypes.h"
#include "Script/Lexer/ScriptLexingTypes.h"

namespace tsumugi::script::lexer { class LexingPosition; }

namespace tsumugi::text::lexer {

/// <summary>
/// トークン
/// </summary>
class Token {
public:
    Token();
    Token(const TokenType token_type, const tstring& literal_string, bool lineHead);
    Token(const TokenType token_type, const tstring& literal_string, bool lineHead, const script::lexer::LexingPosition& position);
    ~Token();

    const TokenType GetTokenType() const;
    void SetTokenType(const TokenType token_type);
        
    const tstring& GetLiteral() const;
    void SetLiteral(const tstring& literal_string);

    bool IsLineHead() const { return isLineHead_; }

private:
    TokenType token_type_;
    tstring literal_;
    script::lexer::LexingPosition position_;
    bool isLineHead_;
};

}

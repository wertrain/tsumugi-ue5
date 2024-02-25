#pragma once

#include "Foundation/Types.h"
#include "Script/Lexing/ScriptTokenTypes.h"
#include "Script/Lexing/ScriptLexingTypes.h"

namespace tsumugi::script::lexing {

/// <summary>
/// ÉgÅ[ÉNÉì
/// </summary>
class Token {
public:
    Token();
    Token(const TokenType token_type, const tstring& literal_string);
    Token(const TokenType token_type, const tstring& literal_string, const LexingPosition& position);

    TokenType GetTokenType() const;
    void SetTokenType(const TokenType token_type);
        
    const tstring& GetLiteral() const;
    void SetLiteral(const tstring& literal_string);

private:
    TokenType token_type_;
    tstring literal_;
    LexingPosition position_;
};

}

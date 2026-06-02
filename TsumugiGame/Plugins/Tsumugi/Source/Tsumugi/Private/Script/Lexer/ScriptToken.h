#pragma once

#include "Foundation/Types.h"
#include "Script/Lexer/ScriptTokenTypes.h"
#include "Script/Lexer/ScriptLexingTypes.h"

namespace tsumugi::script::lexer{

/// <summary>
/// 繝医・繧ｯ繝ｳ
/// </summary>
class Token {
public:
    Token();
    Token(const TokenType token_type, const tstring& literal_string);
    Token(const TokenType token_type, const tstring& literal_string, const LexingPosition& position);
    ~Token();

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

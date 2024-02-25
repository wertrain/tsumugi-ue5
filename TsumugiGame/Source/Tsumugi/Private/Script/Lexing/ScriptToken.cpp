#include "Script/Lexing/ScriptToken.h"
#include <map>

namespace tsumugi::script::lexing {

Token::Token() :
    token_type_(TokenType::kIllegal),
    literal_(),
    position_() {

}

Token::Token(const TokenType token_type, const tstring& literal_string) :
    token_type_(token_type),
    literal_(literal_string),
    position_() {

}

Token::Token(const TokenType token_type, const tstring& literal_string, const LexingPosition& position) :
    token_type_(token_type),
    literal_(literal_string),
    position_(position) {

}

TokenType Token::GetTokenType() const {
    return token_type_;
}

void Token::SetTokenType(const TokenType token_type) {
    token_type_ = token_type;
}

const tstring& Token::GetLiteral() const {
    return literal_;
}

void Token::SetLiteral(const tstring& literal_string) {
    literal_ = literal_string;
}

}

#include "Text/Lexing/TextToken.h"

namespace tsumugi::text::lexing {

Token::Token() :
    token_type_(TokenType::kIllegal),
    literal_() {

}

Token::Token(const TokenType token_type, const tstring& literal_string) :
    token_type_(token_type),
    literal_(literal_string) {

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

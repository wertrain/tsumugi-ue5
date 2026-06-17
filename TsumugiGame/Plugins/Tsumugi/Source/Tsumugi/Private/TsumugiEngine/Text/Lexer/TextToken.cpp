#include "TsumugiEngine/Text/Lexer/TextToken.h"

namespace tsumugi::text::lexer {

Token::Token() :
    token_type_(TokenType::kIllegal),
    literal_(),
    position_(),
    isLineHead_(true) {

}

Token::Token(const TokenType token_type, const tstring& literal_string, bool lineHead) :
    token_type_(token_type),
    literal_(literal_string),
    position_(),
    isLineHead_(lineHead) {

}

Token::Token(const TokenType token_type, const tstring& literal_string, bool lineHead, const script::lexer::LexingPosition& position) :
    token_type_(token_type),
    literal_(literal_string),
    position_(position),
    isLineHead_(lineHead) {

}

Token::~Token() = default;

const TokenType Token::GetTokenType() const {

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

#include "Script/Lexing/ScriptLexer.h"
#include "Script/Lexing/LexingStringReader.h"

namespace tsumugi::script::lexing {

Lexer::Lexer(const tchar* script) :
    reader_(std::make_unique<LexingStringReader>(script)) {
}

Token* Lexer::NextToken() {
    SkipWhiteSpace();
    Token* token = nullptr;

    tchar next = reader_->Peek();
    if (next < 0) {
        token = CreateToken(TokenType::kEOF, tstring());
    } else {
        tchar c = next;
        switch (c) {
            case '=':
                // 次の文字によって、トークンの意味が変わるのでチェックする
                if (reader_->Peek(1) == TT('=')) {
                    // = が二回続けば比較演算子
                    token = CreateToken(TokenType::kEqual, TT("=="));
                    reader_->Read();
                // そうでなければ代入演算子
                } else {
                    token = CreateToken(TokenType::kAssign, tstring(1, c));
                }
                break;
            case TT('+'):
                token = CreateToken(TokenType::kPlus, tstring(1, c));
                break;
            case TT('-'):
                token = CreateToken(TokenType::kMinus, tstring(1, c));
                break;
            case TT('*'):
                token = CreateToken(TokenType::kAsterisk, tstring(1, c));
                break;
            case TT('/'):
                token = CreateToken(TokenType::kSlash, tstring(1, c));
                break;
            case TT('('):
                token = CreateToken(TokenType::kLeftParenthesis, tstring(1, c));
                break;
            case TT(')'):
                token = CreateToken(TokenType::kRightParenthesis, tstring(1, c));
                break;
            case TT('{'):
                token = CreateToken(TokenType::kLeftBraces, tstring(1, c));
                break;
            case TT('}'):
                token = CreateToken(TokenType::kRightBraces, tstring(1, c));
                break;
            case TT(','):
                token = CreateToken(TokenType::kComma, tstring(1, c));
                break;
            case TT(';'):
                token = CreateToken(TokenType::kSemicolon, tstring(1, c));
                break;
            case TT('\0'):
                token = CreateToken(TokenType::kEOF , tstring(1, c));
                break;
        }
    }

    reader_->Read();
    return token;
}

Token* Lexer::CreateToken(const TokenType type, const tstring& literal_string) const {
    return new Token(type, literal_string, reader_->GetLexingPosition());
}

void Lexer::SkipWhiteSpace() {
    tchar next = reader_->Peek();
    while (next == TT(' ')
        || next == TT('\t')
        || next == TT('\r')
        || next == TT('\n')) {
        reader_->Read();
        next = reader_->Peek();
    }
}

}

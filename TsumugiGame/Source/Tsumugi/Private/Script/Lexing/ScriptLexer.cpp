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
        switch (next) {
            case TT('+'):
                token = CreateToken(TokenType::kPlus, tstring());
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

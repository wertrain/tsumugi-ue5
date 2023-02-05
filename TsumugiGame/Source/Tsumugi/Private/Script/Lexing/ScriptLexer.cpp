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
            case TT('!'):
                if (reader_->Peek(1) == TT('=')) {
                    token = CreateToken(TokenType::kNotEqual, tstring(TT("!=")));
                    reader_->Read();
                } else {
                    token = CreateToken(TokenType::kBang, tstring(1, c));
                }
                break;
            case TT('>'):
                if (reader_->Peek(1) == TT('=')) {
                    token = CreateToken(TokenType::kGreaterThanOrEqual, tstring(TT(">=")));
                    reader_->Read();
                } else {
                    token = CreateToken(TokenType::kGreaterThan, tstring(1, c));
                }
                break;
            case TT('<'):
                if (reader_->Peek(1) == TT('=')) {
                    token = CreateToken(TokenType::kLessThanOrEqual, tstring(TT("<=")));
                    reader_->Read();
                } else {
                    token = CreateToken(TokenType::kLessThan, tstring(1, c));
                }
                break;
            case TT('&'):
                if (reader_->Peek(1) == TT('&')) {
                    token = CreateToken(TokenType::kAnd, tstring(TT("&&")));
                    reader_->Read();
                } else {
                    token = CreateToken(TokenType::kLogicalConjunction, tstring(1, c));
                }
                break;
            case TT('|'):
                if (reader_->Peek(1) == TT('|')) {
                    token = CreateToken(TokenType::kOr, tstring(TT("||")));
                    reader_->Read();
                } else {
                    token = CreateToken(TokenType::kLogicalDisjunction , tstring(1, c));
                }
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
            case TT('['):
                token = CreateToken(TokenType::kLeftBrackets, tstring(1, c));
                break;
            case TT(']'):
                token = CreateToken(TokenType::kRightBrackets, tstring(1, c));
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

            default:
                if (IsDigit(c)) {
                    token = CreateAsNumericToken();
                } else if (IsLetter(c)) {
                    tstring identifier;
                    ReadIdentifier(identifier);
                    auto type = LookupIdentifier(identifier);
                    // TODO: TokenType::kIdentifier が帰ってきた場合の処理は大丈夫？
                    token = CreateToken(type, identifier);
                } else {
                    token = CreateToken(TokenType::kIllegal, tstring(1, c));
                }
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

bool Lexer::IsDigit(const tchar c) {
    return TT('0') <= c && c <= TT('9');
}

bool Lexer::IsLetter(const tchar c) {
    /// 引数が（識別子として有効な）文字かをチェックする
    return (TT('a') <= c && c <= TT('z'))
        || (TT('A') <= c && c <= TT('Z'))
        || c == TT('_');
}

void Lexer::ReadNumber(tstring& outNumber) {

    tstring number;
    number.push_back(reader_->Read());

    while (IsDigit(reader_->Peek())) {
        number.push_back(reader_->Read());
    }
    reader_->Seek(-1, script::lexing::LexingStringReader::SeekOrigin::kCurrent);
    
    outNumber = number;
}

void Lexer::ReadIdentifier(tstring& outIdentifier) {

    tstring identifier;
    identifier.push_back(reader_->Read());

    while (IsLetter(reader_->Peek())) {
        identifier.push_back(reader_->Read());
    }
    reader_->Seek(-1, script::lexing::LexingStringReader::SeekOrigin::kCurrent);

    outIdentifier = identifier;
}

Token* Lexer::CreateAsNumericToken() {

    tstring number;
    number.push_back(reader_->Read());

    while (IsDigit(reader_->Peek()) || '.' == reader_->Peek()) {
        number.push_back(reader_->Read());
    }
    reader_->Seek(-1, script::lexing::LexingStringReader::SeekOrigin::kCurrent);

    if (number.find(TT('.')) == tstring::npos) {
        tchar* end;
        [[maybe_unused]] long v = std::wcstol(number.c_str(), &end, 10);
        if (errno != ERANGE && number.c_str() != end) {
            return CreateToken(TokenType::kInteger, number);
        }
    } else {
        tchar* end;
        [[maybe_unused]] double v = std::wcstod(number.c_str(), &end);
        if (errno != ERANGE && number.c_str() != end) {
            return CreateToken(TokenType::kDouble, number);
        }
    }
    return CreateToken(TokenType::kIllegal, number);
}

}

#include "Script/Lexer/ScriptLexer.h"
#include "Script/Lexer/ScriptToken.h"
#include "Script/Lexer/ScriptLexingTypes.h"
#include "Script/Lexer/LexingStringReader.h"

namespace tsumugi::script::lexer {

Lexer::Lexer(const tchar* script)
    : reader_(std::make_unique<LexingStringReader>(script)) {
}

Lexer::~Lexer() = default;

Token* Lexer::NextToken() {
    SkipWhiteSpace();
    Token* token = nullptr;

    tchar next = reader_->Peek();
    if (next < 0) {
        token = CreateToken(TokenType::kEOF, tstring());
    } else {
        tchar c = next;
        switch (c) {
            case TT('='):
                // 谺｡縺ｮ譁・ｭ励↓繧医▲縺ｦ縲√ヨ繝ｼ繧ｯ繝ｳ縺ｮ諢丞袖縺悟､峨ｏ繧九・縺ｧ繝√ぉ繝・け縺吶ｋ
                if (reader_->Peek(1) == TT('=')) {
                    // = 縺御ｺ悟屓邯壹￠縺ｰ豈碑ｼ・ｼ皮ｮ怜ｭ・
                    token = CreateToken(TokenType::kEqual, TT("=="));
                    reader_->Read();
                // 縺昴≧縺ｧ縺ｪ縺代ｌ縺ｰ莉｣蜈･貍皮ｮ怜ｭ・
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
            case TT(':'):
                token = CreateToken(TokenType::kColon, tstring(1, c));
                break;
            case TT('\"'):
            case TT('\''):
                token = CreateAsStringToken();
                break;
            case TT('\0'):
                token = CreateToken(TokenType::kEOF , tstring(1, c));
                break;
            case TT('.'):
                token = CreateToken(TokenType::kDot, tstring(1, c));
                break;
            case TT('#'):
                token = CreateToken(TokenType::kSharp, tstring(1, c));
                break;
            default:
                if (IsDigit(c)) {
                    token = CreateAsNumericToken();
                } else if (IsLetter(c)) {
                    tstring identifier;
                    ReadIdentifier(identifier);
                    auto type = LookupIdentifier(identifier);
                    // TODO: TokenType::kIdentifier 縺悟ｸｰ縺｣縺ｦ縺阪◆蝣ｴ蜷医・蜃ｦ逅・・螟ｧ荳亥､ｫ・・
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

Token* Lexer::CreateToken(const TokenType type, const tstring& literalString) const {
    return new Token(type, literalString, reader_->GetLexingPosition());
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
    /// 蠑墓焚縺鯉ｼ郁ｭ伜挨蟄舌→縺励※譛牙柑縺ｪ・画枚蟄励°繧偵メ繧ｧ繝・け縺吶ｋ
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
    reader_->Seek(-1, script::lexer::LexingStringReader::SeekOrigin::kCurrent);
    
    outNumber = number;
}

void Lexer::ReadIdentifier(tstring& outIdentifier) {

    tstring identifier;
    identifier.push_back(reader_->Read());

    while (IsLetter(reader_->Peek()) || IsDigit(reader_->Peek())) {
        identifier.push_back(reader_->Read());
    }
    reader_->Seek(-1, script::lexer::LexingStringReader::SeekOrigin::kCurrent);

    outIdentifier = identifier;
}

Token* Lexer::CreateAsNumericToken() {

    tstring number;
    number.push_back(reader_->Read());

    while (IsDigit(reader_->Peek()) || '.' == reader_->Peek()) {
        number.push_back(reader_->Read());
    }
    reader_->Seek(-1, script::lexer::LexingStringReader::SeekOrigin::kCurrent);

    if (number.find(TT('.')) == tstring::npos) {
        [[maybe_unused]] long v = 0;
        if (tsumugi::type::convert::FromChars(number, v)) {
            return CreateToken(TokenType::kInteger, number);
        }
    } else {
        [[maybe_unused]] double v = 0;
        if (tsumugi::type::convert::FromChars(number, v)) {
            return CreateToken(TokenType::kFloat, number);
        }
    }
    return CreateToken(TokenType::kIllegal, number);
}

Token* Lexer::CreateAsStringToken() {

    tchar quote = reader_->Read();

    tstring str;

    while (true) {
        auto ch = reader_->Read();

        if (ch == 0) {
            // 邨らｫｯ縺ｾ縺ｧ髢峨§繧ｯ繧ｩ繝ｼ繝医′辟｡縺・竊・繧ｨ繝ｩ繝ｼ
            return CreateToken(TokenType::kIllegal, str);
        }

        if (ch == quote) {
            // 蜷後§繧ｯ繧ｩ繝ｼ繝医〒髢峨§繧・
            break;
        }

        // 繧ｨ繧ｹ繧ｱ繝ｼ繝怜ｯｾ蠢懊＠縺溘＞縺ｪ繧峨％縺薙〒蜃ｦ逅・
        if (ch == '\\') {
            auto next = reader_->Read();
            switch (next) {
                case 'n':  str.push_back('\n'); break;
                case 't':  str.push_back('\t'); break;
                case '\\': str.push_back('\\'); break;
                case '"':  str.push_back('"'); break;
                case '\'': str.push_back('\''); break;
                default:
                    str.push_back(next);
                    break;
            }
            continue;
        }

        str.push_back(ch);
    }
    reader_->Seek(-1, script::lexer::LexingStringReader::SeekOrigin::kCurrent);

    return CreateToken(TokenType::kString, str);
}

}

#include "Text/Lexer/TextLexer.h"
#include "Text/Lexer/TextToken.h"
#include "Script/Lexer/ScriptLexingTypes.h"
#include "Script/Lexer/LexingStringReader.h"

namespace tsumugi::text::lexer {

/// <summary>
/// 繧ｳ繝ｳ繝医Ο繝ｼ繝ｫ譁・ｭ励・螳夂ｾｩ
/// </summary>
enum SyntaxSymbol : int {

    /// <summary>
    /// 繝ｩ繝吶Ν縺ｮ髢句ｧ区枚蟄・
    /// </summary>
    Colon = ':',

    /// <summary>
    /// 陦碁ｭ縺ｮ繝ｩ繝吶Ν螳夂ｾｩ繝槭・繧ｯ
    /// </summary>
    Asterisk = '*',

    /// <summary>
    /// 陦碁ｭ縺ｮ繧ｳ繝槭Φ繝牙ｮ夂ｾｩ繝槭・繧ｯ
    /// </summary>
    AtMark = '@',

    /// <summary>
    /// 繝ｩ繝吶Ν縺ｮ隕句・縺励そ繝代Ξ繝ｼ繧ｿ繝ｼ
    /// </summary>
    HeadlineSeparator = '|',

    /// <summary>
    /// 繧ｿ繧ｰ縺ｮ髢句ｧ九き繝・さ
    /// </summary>
    TagStart = '[',

    /// <summary>
    /// 繧ｿ繧ｰ縺ｮ邨ゆｺ・き繝・さ
    /// </summary>
    TagEnd = ']',

    /// <summary>
    /// 繧ｿ繧ｰ螻樊ｧ縺ｮ莉｣蜈･譁・ｭ・
    /// </summary>
    Assignment = '=',

    /// <summary>
    /// 陦碁ｭ縺ｮ繧ｳ繝｡繝ｳ繝医・繝ｼ繧ｯ
    /// </summary>
    SemiColon = ';',
};

Lexer::Lexer(const tchar* script)
    : reader_(std::make_unique<script::lexer::LexingStringReader>(script))
    , isLineHead_(true) {
}

Lexer::~Lexer() = default;

Token* Lexer::NextToken() {

    Token* token = nullptr;

    SkipWhiteSpace();

    bool currentIsLineHead = isLineHead_;
    isLineHead_ = false;

    tchar next = reader_->Peek();
    if (next < 0) {
        token = CreateToken(TokenType::kEOF, tstring(), currentIsLineHead);
    } else {
        tchar c = next;
        switch (c) {
            case SyntaxSymbol::Asterisk: {
                token = CreateToken(TokenType::kAsterisk, tstring(1, c), currentIsLineHead);
                break;
            }
            case SyntaxSymbol::AtMark: {
                token = CreateToken(TokenType::kAtMark, tstring(1, c), currentIsLineHead);
                break;
            }
            case SyntaxSymbol::HeadlineSeparator: {
                token = CreateToken(TokenType::kPipe, tstring(1, c), currentIsLineHead);
                break;
            }
            case SyntaxSymbol::TagStart: {
                token = CreateToken(TokenType::kTagOpen, tstring(1, c), currentIsLineHead);
                break;
            }
            case SyntaxSymbol::TagEnd: {
                token = CreateToken(TokenType::kTagClose, tstring(1, c), currentIsLineHead);
                break;
            }
            case SyntaxSymbol::Assignment: {
                token = CreateToken(TokenType::kAssign, tstring(1, c), currentIsLineHead);
                break;
            }
            case SyntaxSymbol::SemiColon: {
                token = CreateToken(TokenType::kSemiColon, tstring(1, c), currentIsLineHead);
                break;
            }
            case '\n': {
                token = CreateToken(TokenType::kNewLine, tstring(1, c), currentIsLineHead);
                isLineHead_ = true;
                break;
            }
            case '\r': {
                tstring newline = tstring(1, c);
                if (reader_->Peek(1) == '\n') {
                    newline.push_back(reader_->Peek(1));
                    reader_->Read();
                }
                token = CreateToken(TokenType::kNewLine, newline, currentIsLineHead);
                isLineHead_ = true;
                break;
            }
            default: {
                tstring out;
                ReadString(out);
                token = CreateToken(TokenType::kString, out, currentIsLineHead);
                break;
            }
        }
    }

    reader_->Read();
    return token;
}

Token* Lexer::CreateToken(const TokenType type, const tstring& literalString, bool isLineHead) const {

    return new Token(type, literalString, isLineHead, reader_->GetLexingPosition());
}

void Lexer::SkipWhiteSpace() const {

    tchar next = reader_->Peek();
    while (IsWhiteSpace(next)) {
        reader_->Read();
        next = reader_->Peek();
    }
}

bool Lexer::IsWhiteSpace(tchar c) const {

    return c == TT(' ')
        || c == TT('\t');
}

bool Lexer::IsSyntaxSymbol(tchar c) const {

    return c == SyntaxSymbol::Colon
        || c == SyntaxSymbol::Asterisk
        || c == SyntaxSymbol::AtMark
        || c == SyntaxSymbol::HeadlineSeparator
        || c == SyntaxSymbol::TagStart
        || c == SyntaxSymbol::TagEnd
        || c == SyntaxSymbol::Assignment
        || c == SyntaxSymbol::SemiColon;
}

bool Lexer::IsDigit(const tchar c) const {

    return TT('0') <= c && c <= TT('9');
}

bool Lexer::IsLetter(const tchar c) const {

    /// 蠑墓焚縺鯉ｼ郁ｭ伜挨蟄舌→縺励※譛牙柑縺ｪ・画枚蟄励°繧偵メ繧ｧ繝・け縺吶ｋ
    return (TT('a') <= c && c <= TT('z'))
        || (TT('A') <= c && c <= TT('Z'))
        || c == TT('_');
}

void Lexer::ReadString(tstring& out) const {

    tstring text;
    text.push_back(reader_->Read());

    while (!IsWhiteSpace(reader_->Peek()) &&
           !IsSyntaxSymbol(reader_->Peek()) &&
           (reader_->Peek() > 0) &&
           (reader_->Peek() != '\n') &&
           (reader_->Peek() != '\r')) {
        text.push_back(reader_->Read());
    }
    reader_->Seek(-1, script::lexer::LexingStringReader::SeekOrigin::kCurrent);

    out = text;
}

}

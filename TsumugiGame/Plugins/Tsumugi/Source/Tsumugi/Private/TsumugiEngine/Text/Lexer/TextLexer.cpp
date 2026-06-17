#include "TsumugiEngine/Text/Lexer/TextLexer.h"
#include "TsumugiEngine/Text/Lexer/TextToken.h"
#include "TsumugiEngine/Script/Lexer/ScriptLexingTypes.h"
#include "TsumugiEngine/Script/Lexer/LexingStringReader.h"

namespace tsumugi::text::lexer {

/// <summary>
/// コントロール文字の定義
/// </summary>
enum SyntaxSymbol : int {

    /// <summary>
    /// ラベルの開始文字
    /// </summary>
    Colon = ':',

    /// <summary>
    /// 行頭のラベル定義マーク
    /// </summary>
    Asterisk = '*',

    /// <summary>
    /// 行頭のコマンド定義マーク
    /// </summary>
    AtMark = '@',

    /// <summary>
    /// ラベルの見出しセパレーター
    /// </summary>
    HeadlineSeparator = '|',

    /// <summary>
    /// タグの開始カッコ
    /// </summary>
    TagStart = '[',

    /// <summary>
    /// タグの終了カッコ
    /// </summary>
    TagEnd = ']',

    /// <summary>
    /// タグ属性の代入文字
    /// </summary>
    Assignment = '=',

    /// <summary>
    /// 行頭のコメントマーク
    /// </summary>
    SemiColon = ';',

    /// <summary>
    /// ダブルクォーテーション
    /// </summary>
    DoubleQuote = '"',

    /// <summary>
    /// シングルクォーテーション
    /// </summary>
    SingleQuote = '\'',
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
    if (next <= 0) {
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
            case SyntaxSymbol::DoubleQuote:
            case SyntaxSymbol::SingleQuote: {
                tstring out;
                ReadQuotedString(out, c);
                token = CreateToken(TokenType::kString, out, currentIsLineHead);
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

    /// 引数が（識別子として有効な）文字かをチェックする
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

void Lexer::ReadQuotedString(tstring& out, tchar quoteChar) const {

    tstring text;

    reader_->Read();

    while (reader_->Peek() > 0) {
        tchar next = reader_->Peek();
        if (next == quoteChar) {
            break;
        }
        if (next == '\n' || next == '\r') {
            break;
        }
        text.push_back(reader_->Read());
    }
    if (reader_->Peek() == quoteChar) {
        reader_->Read();
    }

    reader_->Seek(-1, script::lexer::LexingStringReader::SeekOrigin::kCurrent);

    out = text;
}

bool Lexer::ReadRawUntil(tstring& out, const tstring& target) {

    tstring result;

    tchar targetBegin = target.front();
    tchar targetEnd = target.back();

    while (true) {

        tchar c = reader_->Peek();

        // EOF
        if (c <= 0) {
            return false;
        }

        // '[' を検出したらタグ判定
        if (c == targetBegin) {
            // 位置を保存
            auto pos = reader_->GetPosition();

            tstring readText;
            readText.push_back(reader_->Read());
            while (reader_->Peek() > 0 && reader_->Peek() != targetEnd && !IsWhiteSpace(reader_->Peek())) {
                readText.push_back(reader_->Read());
            }
            // targetEnd までスキップ
            while (reader_->Peek() > 0 && reader_->Peek() != targetEnd) {
                reader_->Read();
            }
            if (reader_->Peek() == targetEnd) {
                readText.push_back(reader_->Read());
            }
            if (readText == target) {
                break;
            }
            // 違うなら元に戻してそのまま出力
            reader_->Seek(pos, script::lexer::LexingStringReader::SeekOrigin::kBegin);
            result.push_back(reader_->Read());
            continue;
        }
        // 通常文字
        result.push_back(reader_->Read());
    }
    out = result;
    return true;
}

}

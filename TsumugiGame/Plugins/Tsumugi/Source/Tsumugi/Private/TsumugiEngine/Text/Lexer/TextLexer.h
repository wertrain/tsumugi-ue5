#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Text/Lexer/TextTokenTypes.h"

namespace tsumugi::text::lexer { class Token; }
namespace tsumugi::script::lexer { class LexingStringReader; }

namespace tsumugi::text::lexer{

class Lexer {
public:
    Lexer(const tchar* script);
    ~Lexer();
    Token* NextToken();

private:
    Token* CreateToken(const TokenType type, const tstring& literalString, bool isLineHead) const;
    void SkipWhiteSpace() const;
    bool IsWhiteSpace(tchar c) const;
    bool IsSyntaxSymbol(tchar c) const;
    bool IsDigit(tchar c) const;
    bool IsLetter(tchar c) const;
    void ReadString(tstring& out) const;
    void ReadQuotedString(tstring& out, tchar quoteChar) const;

public:
    bool ReadRawUntil(tstring& out, const tstring& target);

private:
    std::unique_ptr<script::lexer::LexingStringReader> reader_;
    bool isLineHead_;
};

}

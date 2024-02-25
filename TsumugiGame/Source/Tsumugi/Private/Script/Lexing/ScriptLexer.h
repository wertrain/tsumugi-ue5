#pragma once

#include "Foundation/Types.h"
#include "Script/Lexing/ScriptTokenTypes.h"

namespace tsumugi::script::lexing { class Token; }
namespace tsumugi::script::lexing { class LexingStringReader; }

namespace tsumugi::script::lexing {

class Lexer {
public:
    Lexer(const tchar* script);
    Token* NextToken();

private:
    Token* CreateToken(const TokenType type, const tstring& literal_string) const;
    void SkipWhiteSpace();
    bool IsDigit(tchar c);
    bool IsLetter(tchar c);
    void ReadNumber(tstring& outNumber);
    void ReadIdentifier(tstring& outIdentifier);
    Token* CreateAsNumericToken();

private:
    std::unique_ptr<LexingStringReader> reader_;
};

}

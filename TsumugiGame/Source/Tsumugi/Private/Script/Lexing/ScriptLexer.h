#pragma once

#include <cstdlib>
#include <memory>
#include "Foundation/Types.h"
#include "Script/Lexing/ScriptToken.h"
#include "Script/Lexing/ScriptLexingTypes.h"

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
    std::unique_ptr<class LexingStringReader> reader_;
};

}

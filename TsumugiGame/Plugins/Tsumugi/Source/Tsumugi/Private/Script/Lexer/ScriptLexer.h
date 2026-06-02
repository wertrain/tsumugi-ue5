#pragma once

#include "Foundation/Types.h"
#include "Script/Lexer/ScriptTokenTypes.h"

namespace tsumugi::script::lexer{ class Token; }
namespace tsumugi::script::lexer{ class LexingStringReader; }

namespace tsumugi::script::lexer{

class Lexer {
public:
    Lexer(const tchar* script);
    ~Lexer();
    Token* NextToken();

private:
    Token* CreateToken(const TokenType type, const tstring& literal_string) const;
    void SkipWhiteSpace();
    bool IsDigit(tchar c);
    bool IsLetter(tchar c);
    void ReadNumber(tstring& outNumber);
    void ReadIdentifier(tstring& outIdentifier);
    Token* CreateAsNumericToken();
    Token* CreateAsStringToken();

private:
    std::unique_ptr<LexingStringReader> reader_;
};

}

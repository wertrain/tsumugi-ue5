#include <iostream>
#include "Script/Lexing/ScriptLexer.h"
#include "Script/Lexing/ScriptToken.h"
#include "Script/Lexing/LexingStringReader.h"

int main()
{
    std::cout << "Hello Tsumugi Script!" << std::endl;

    const char* PROMPT = ">> ";

    while (true)
    {
        std::cout << PROMPT;

        wchar_t input[1024];
        std::wcin >> input;

        if (wcslen(input) == 0) {
            return 0;
        }

        tsumugi::script::lexing::Lexer lexer(input);
        for (auto* token = lexer.NextToken(); token->GetTokenType() != tsumugi::script::lexing::TokenType::kEOF; token = lexer.NextToken()) {
            std::wcout << "{ Type: " << tsumugi::script::lexing::TokenTypeToString(token->GetTokenType()) << ", Literal: " << token->GetLiteral() << " }"<< std::endl;
            delete token;
        }
    }
    return 0;
}

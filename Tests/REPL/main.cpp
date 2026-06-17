#if 1
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <iostream>
#include "TsumugiEngine/Script/Lexer/ScriptLexer.h"
#include "TsumugiEngine/Script/Parser/ScriptParser.h"
#include "TsumugiEngine/Script/Lexer/ScriptToken.h"
#include "TsumugiEngine/Script/Lexer/LexingStringReader.h"
#include "TsumugiEngine/Script/Evaluator/Evaluator.h"
#include "TsumugiEngine/Script/AST/Program.h"
#include "TsumugiEngine/Script/Objects/IObject.h"
#include "TsumugiEngine/Script/Objects/BooleanObject.h"
#include "TsumugiEngine/Script/Objects/Environment.h"

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    std::cout << "Hello Tsumugi Script!" << std::endl;

    const char* PROMPT = ">> ";

    auto environment = std::make_shared<tsumugi::script::object::Environment>();

    while (true)
    {
        std::cout << PROMPT;

        tstring input;
        std::getline(std::wcin, input);

        if (input.size() == 0) {
            break;
        }

        auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(input.c_str());
        auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
        auto root = parser->ParseProgram();

        if (parser->GetLogger().HasAnyLog()) {
            continue;
        }

        auto evaluator = std::make_unique<tsumugi::script::evaluator::Evaluator>();
        auto evaluated = evaluator->Eval(root.get(), environment);
        if (evaluated != nullptr && evaluated->GetType() != tsumugi::script::object::ObjectType::kNull) {
            tout() << evaluated->Inspect() << tendl;
        }
    }

    environment->Clear();

    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);

    return 0;
}

#else

#include <iostream>
#include "Script/Lexing/ScriptLexer.h"
#include "Script/Parsing/ScriptParser.h"
#include "Script/Lexing/ScriptToken.h"
#include "Script/Lexing/LexingStringReader.h"
#include "Script/AbstractSyntaxTree/Root.h"

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    {
        tstring letcode =
            TT("foobar;");

        auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(letcode.c_str());
        auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
        auto root = parser->ParseProgram();
    }

    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    // static な map の誤検出防止
    //_CrtDumpMemoryLeaks();

    return 0;
}

#endif

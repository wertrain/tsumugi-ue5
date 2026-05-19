#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include "Script/Lexer/ScriptLexer.h"
#include "Script/Parser/ScriptParser.h"
#include "Script/Lexer/ScriptToken.h"
#include "Script/Lexer/LexingStringReader.h"
#include "Script/Evaluator/Evaluator.h"
#include "Script/AST/Root.h"
#include "Script/Objects/IObject.h"
#include "Script/Objects/BooleanObject.h"
#include "Script/Objects/Environment.h"


void TypeOut(const std::wstring& s, int delay = 20)
{
    for (auto c : s) {
        std::wcout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}

void ShowEnter()
{
    std::wcout << L"[Enter]" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
}

void RunOneLine(const tstring& input,
    std::shared_ptr<tsumugi::script::object::Environment> env)
{
    auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(input.c_str());
    auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
    auto root = parser->ParseProgram();

    if (parser->GetLogger().HasAnyLog()) {
        return;
    }

    auto evaluator = std::make_unique<tsumugi::script::evaluator::Evaluator>();
    auto evaluated = evaluator->Eval(root.get(), env);
    if (evaluated != nullptr) {
        tout() << evaluated->Inspect() << tendl;
    }
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "Usage: tsumugi-demo <scriptfile>" << std::endl;
        return 1;
    }

    std::wifstream script(argv[1]);
    if (!script) {
        std::cout << "Cannot open script file." << std::endl;
        return 1;
    }

    auto env = std::make_shared<tsumugi::script::object::Environment>();

    std::wstring line;
    while (std::getline(script, line)) {

        // 入力行をタイプアウト
        std::wcout << L">> ";
        TypeOut(line);
        std::wcout << std::endl;

        // Enter 演出
        ShowEnter();

        // 実行
        RunOneLine(line, env);

        // 少し待つ（動画のテンポ調整）
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    return 0;
}

#define _CRTDBG_MAP_ALLOC
#include "Foundation/Types.h"
#include "Text/Lexer/TextLexer.h"
#include "Text/Parser/TextParser.h"
#include "Text/Lexer/TextToken.h"
#include "Text/Evaluator/TextEvaluator.h"
#include "Text/AST/TextProgram.h"
#include "ConsoleGameContext.h"
#include <crtdbg.h>

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	std::cout << "Hello Tsumugi Text!" << std::endl;

	const char* PROMPT = ">> ";

	while (true)
	{
		std::cout << PROMPT;

		tstring input;
		std::getline(std::wcin, input);

		if (input.size() == 0) {
			break;
		}

		tsumugi::text::lexer::Lexer lexer(input.c_str());
		tsumugi::text::parser::Parser parser(&lexer);
		auto program = parser.ParseProgram();

		ConsoleGameContext ctx;
		tsumugi::text::evaluator::Evaluator eval(ctx);

		eval.Execute(*program);
	}

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);

	return 0;
}

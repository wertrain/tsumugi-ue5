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

	tstring sample = LR"(
		[cm]
		[font size=28 color=0x00AAFF]
		[delay speed=20]
		ようこそ、Tsumugi ノベルエンジンへ。

		これはコンソール版のテストシナリオです。
		文字送り、改行、ページ送り、フォント変更などを確認できます。[p]

		[font size=24 color=0xFFFFFF]
		[delay speed=40]
		まずは、基本的なテキスト表示から始めましょう。
		この文章は 40ms ごとに 1 文字表示されます。
		[p]

		[font color=0xFF4444]
		[delay speed=10]
		色を変えることもできます。
		速度も速くできます。
		[p]

		[font color=0x44FF44]
		[delay speed=80]
		逆に、ゆっくりにすることもできます……。
		……ほらね。
		[p]

		[cm]
		[font size=28 color=0xFFFFFF]
		[delay speed=nowait]
		画面をクリアしました。
		この文章は nowait なので一瞬で表示されます。[p]

		[delay speed=user]
		ユーザー設定速度に戻しました。
		[p]

		[cm]
		[font size=30 color=0xFFAA00]
		最後に、少し長めの文章を表示してみましょう。

		[delay speed=25]
		静かな夜だった。
		街灯の下を歩くと、影がゆらゆらと揺れる。
		遠くで電車の音が響き、風が頬を撫でていく。
		
		「……明日も、頑張ろう。」
		[p]

		[cm]
		[font size=26 color=0xFFFFFF]
		テストは以上です。
		ご協力ありがとうございました。[p]
	)";

	std::wcout << "Hello Tsumugi Text!" << std::endl;

	tsumugi::text::lexer::Lexer lexer(sample.c_str());
	tsumugi::text::parser::Parser parser(&lexer);
	auto program = parser.ParseProgram();

	ConsoleGameContext ctx;
	tsumugi::text::evaluator::Evaluator eval(ctx);

	eval.Execute(*program);

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);

	return 0;
}

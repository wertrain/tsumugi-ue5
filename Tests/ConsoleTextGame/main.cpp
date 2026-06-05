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

	sample = LR"(
		[cm]
		[font size = 28 color = 0x88CCFF]
		[delay speed = 20]
		; ---------------------------------------- -
		; 初期化
		; ---------------------------------------- -
		[eval exp = "f.name = 'ユウ'"]
		[eval exp = "f.hp = 30"]
		[eval exp = "f.maxhp = 30"]
		[eval exp = "f.gold = 120"]
		[eval exp = "f.potion = 1"]
		ようこそ、旅人[emb exp = "f.name"]。
		あなたの冒険が、今ここから始まります。[p]
		
		[font size = 24 color = 0xFFFFFF]
		現在のステータス：
		HP : [emb exp = "f.hp"] / [emb exp = "f.maxhp"]
		所持金 : [emb exp = "f.gold"] G
		ポーション : [emb exp = "f.potion"] 個[p]

		; ---------------------------------------- -
		; スライム戦（固定演出）
		; ---------------------------------------- -
		[cm]
		[font size = 26 color = 0xFFAA66]
		森の中で、スライムが現れた！

		[eval exp = "f.slime_hp = 12"]
		スライムのHP : [emb exp = "f.slime_hp"] [p]

		; ダメージ計算（固定）
		[eval exp = "f.damage = 5 + 3"]
		[eval exp = "f.slime_hp = f.slime_hp - f.damage"]

		あなたはスライムに[emb exp = "f.damage"] のダメージを与えた！
		スライムの残りHP : [emb exp = "f.slime_hp"] 

		; スライム撃破（固定演出）
		スライムを倒した！
		[eval exp = "f.gold = f.gold + 30"]
		[eval exp = "f.exp = 10"]
		報酬 : 30G、経験値10[p]

		; ---------------------------------------- -
		; 回復（固定演出）
		; ---------------------------------------- -
		[cm]
		[font size = 26 color = 0x88FFAA]
		戦闘後、あなたは休憩することにした。

		ポーションを使用した。
		[eval exp = "f.potion = f.potion - 1"]
		[eval exp = "f.hp = f.maxhp"]

		HP が全回復した！
		現在のHP : [emb exp = "f.hp"] / [emb exp = "f.maxhp"]
		残りポーション : [emb exp = "f.potion"] [p]

		; ---------------------------------------- -
		; 買い物（固定演出）
		; ---------------------------------------- -
		[cm]
		[font size = 26 color = 0xFFDD88]
		村に到着した。
		商人が話しかけてくる。

		「ポーションは 50G だよ！」

		; 条件分岐なし → 強制購入
		[eval exp = "f.gold = f.gold - 50; f.potion = f.potion + 1"]
		あなたはポーションを購入した。

		現在の所持金 : [emb exp = "f.gold"] G
		ポーション : [emb exp = "f.potion"] 個[p]

		; ---------------------------------------- -
		; iscript による複数行 TJS
		; ---------------------------------------- -
		[cm]
		[font size = 26 color = 0xFFFFFF]
		[iscript]
		f.msg = "今日の冒険はここまでだ。";
		f.msg = f.msg + " また明日も頑張ろう。";
		[endscript]
		[emb exp = "f.msg"] 
		[p]

		[cm]
		[font size = 30 color = 0x88CCFF]
		これで eval / emb のテストは完了です。
		ご協力ありがとうございました。[p])";

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

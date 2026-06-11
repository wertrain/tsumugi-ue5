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
		[cm][font size=28 color=0x88CCFF][delay speed=20]
		[eval exp="f.name = 'ユウ'"]
		[eval exp="f.hp = 30"]
		[eval exp="f.maxhp = 30"]
		[eval exp="f.gold = 70"]
		[eval exp="f.potion = 1"]
		ようこそ、旅人 [emb exp="f.name"]。[r]
		あなたの冒険が、今ここから始まります。[p]
		[font size=24 color=0xFFFFFF]
		現在のステータス：[r]
		HP : [emb exp="f.hp"] / [emb exp="f.maxhp"][r]
		所持金 : [emb exp="f.gold"] G[r]
		ポーション : [emb exp="f.potion"] 個[p]
		[cm]
		[font size=26 color=0xFFAA66]
		森の中で、スライムが現れた！[r]
		[delay speed=30]
		[eval exp="f.slime_hp = 12"]
		スライムのHP : [emb exp="f.slime_hp"][p]
		[font color=0xFFFFFF]
		; 固定ダメージ
		[eval exp="f.damage = Random.int(8, 15) "]
		[eval exp="f.slime_hp = max(0, f.slime_hp - f.damage) "]
		あなたはスライムに [emb exp="f.damage"] のダメージを与えた！[r]
		スライムの残りHP : [emb exp="f.slime_hp"][p]

		[if exp="f.slime_hp <= 0"]
		スライムを倒した！[r]
		[eval exp="f.gold = f.gold + 30"]
		[eval exp="f.exp = 10"]
		報酬 : 30G、経験値10[r]
		[endif]

		[if exp="f.slime_hp > 0"]
		スライムはまだ生きている……。[r]
		しかし、スライムは逃げていった。[r]
		[endif]
		[p]

		[cm]
		[font size=26 color=0x88FFAA]
		戦闘後、あなたは休憩することにした。[r]

		[if exp="f.potion > 0"]
		ポーションを使いますか？[r]
		（自動で使用します）[r]
		[eval exp="f.potion = f.potion - 1"]
		[eval exp="f.hp = f.maxhp"]
		HP が全回復した！[r]
		[endif]

		現在のHP : [emb exp="f.hp"] / [emb exp="f.maxhp"][r]
		残りポーション : [emb exp="f.potion"][p]

		[cm]
		[font size=26 color=0xFFDD88]
		村に到着した。[r]
		商人が話しかけてくる。[r]

		「ポーションは 50G だよ！」[r]

		[if exp="f.gold >= 50"]
		[eval exp="f.gold = f.gold - 50;f.potion = f.potion + 1"]
		あなたはポーションを購入した。[r]
			「もう一つ買うかい？」[r]
			[if exp="f.gold >= 50"]
			[eval exp="f.gold = f.gold - 50;f.potion = f.potion + 1"]
			あなたはポーションを購入した。[r]
			[else]
			お金が足りない……。[r]
			[endif]
		[else]
		お金が足りない……。[r]
		[endif]

		現在の所持金 : [emb exp="f.gold"] G[r]
		ポーション : [emb exp="f.potion"] 個[p]

		[cm]
		[font size=26 color=0xFFFFFF]
		[iscript]
		f.msg = "今日の冒険はここまでだ。";
		f.msg = f.msg + " また明日も頑張ろう。";
		[endscript]

		[emb exp="f.msg"][r]
		[p]

		[cm]
		[font size=30 color=0x88CCFF]
		これで eval / emb のテストは完了です。[r]
		ご協力ありがとうございました。[p])";

	sample = LR"(
		*start
		[cm]
		条件によってサブルーチン内でジャンプします。[r]

		[eval exp="f.flag = 0"]
		; f.flag を 0 / 1 / 2 に変えて動作確認できます

		[call target=*check]

		戻ってきました。[r]
		[s]

		;---------------------------------------
		; サブルーチン本体
		;---------------------------------------
		*check
		[if exp="f.flag == 1"]
			f.flag == 1 の処理です。[r]
			[jump target=*path1]
		[endif]

		[if exp="f.flag == 2"]
			f.flag == 2 の処理です。[r]
			[jump target=*path2]
		[endif]

		どちらでもないので return。[r]
		[return]

		;---------------------------------------
		; サブルーチンの分岐先
		;---------------------------------------
		*path1
		flag1 の処理です。[r]
		[return]

		*path2
		flag2 の処理です。[r]
		[return]
	)";

	sample = LR"(
		*start
		[cm]
		if / elsif / else のテストを開始します。[r]

		;---------------------------------------
		; テスト 1：if が true
		;---------------------------------------
		[eval exp="f.x = 0"]
		[cm]
		テスト1：f.x = 0 の場合。[r]

		[if exp="f.x == 0"]
			IF ブロックが実行されました。[r]
		[elsif exp="f.x == 1"]
			ELSIF ブロック（出てはいけない）。[r]
		[else]
			ELSE ブロック（出てはいけない）。[r]
		[endif]

		次へ。[r]


		;---------------------------------------
		; テスト 2：elsif が true
		;---------------------------------------
		[eval exp="f.x = 1"]
		[cm]
		テスト2：f.x = 1 の場合。[r]

		[if exp="f.x == 0"]
			IF ブロック（出てはいけない）。[r]
		[elsif exp="f.x == 1"]
			ELSIF ブロックが実行されました。[r]
		[else]
			ELSE ブロック（出てはいけない）。[r]
		[endif]

		次へ。[r]


		;---------------------------------------
		; テスト 3：else が実行される
		;---------------------------------------
		[eval exp="f.x = 2"]
		[cm]
		テスト3：f.x = 2 の場合。[r]

		[if exp="f.x == 0"]
			IF ブロック（出てはいけない）。[r]
		[elsif exp="f.x == 1"]
			ELSIF ブロック（出てはいけない）。[r]
		[else]
			ELSE ブロックが実行されました。[r]
		[endif]

		テスト終了。[r]
		[s]
	)";

	sample = LR"(
		*start
		[cm]
		ネスト if / elsif / else のテストを開始します。[r]

		;---------------------------------------
		; テスト 1：外側 if が true
		;---------------------------------------
		[eval exp="f.x = 0"]
		[eval exp="f.y = 1"]
		[cm]
		テスト1：f.x=0, f.y=1 の場合。[r]

		[if exp="f.x == 0"]
			外側 IF が実行されました。[r]

			[if exp="f.y == 1"]
				内側 IF が実行されました。[r]
			[elsif exp="f.y == 2"]
				内側 ELSIF（出てはいけない）。[r]
			[else]
				内側 ELSE（出てはいけない）。[r]
			[endif]

		[elsif exp="f.x == 1"]
			外側 ELSIF（出てはいけない）。[r]
		[else]
			外側 ELSE（出てはいけない）。[r]
		[endif]

		次へ。[r]


		;---------------------------------------
		; テスト 2：外側 elsif が true
		;---------------------------------------
		[eval exp="f.x = 1"]
		[eval exp="f.y = 2"]
		[cm]
		テスト2：f.x=1, f.y=2 の場合。[r]

		[if exp="f.x == 0"]
			外側 IF（出てはいけない）。[r]

			[if exp="f.y == 1"]
				内側 IF（出てはいけない）。[r]
			[else]
				内側 ELSE（出てはいけない）。[r]
			[endif]

		[elsif exp="f.x == 1"]
			外側 ELSIF が実行されました。[r]

			[if exp="f.y == 2"]
				内側 IF が実行されました。[r]
			[else]
				内側 ELSE（出てはいけない）。[r]
			[endif]

		[else]
			外側 ELSE（出てはいけない）。[r]
		[endif]

		次へ。[r]


		;---------------------------------------
		; テスト 3：外側 else が実行される
		;---------------------------------------
		[eval exp="f.x = 2"]
		[eval exp="f.y = 3"]
		[cm]
		テスト3：f.x=2, f.y=3 の場合。[r]

		[if exp="f.x == 0"]
			外側 IF（出てはいけない）。[r]
		[elsif exp="f.x == 1"]
			外側 ELSIF（出てはいけない）。[r]
		[else]
			外側 ELSE が実行されました。[r]

			[if exp="f.y == 1"]
				内側 IF（出てはいけない）。[r]
			[elsif exp="f.y == 2"]
				内側 ELSIF（出てはいけない）。[r]
			[else]
				内側 ELSE が実行されました。[r]
			[endif]

		[endif]

		テスト終了。[r]
		[s]
	)";

	std::wcout << "Hello Tsumugi Text!" << std::endl;

	tsumugi::text::lexer::Lexer lexer(sample.c_str());
	tsumugi::text::parser::Parser parser(&lexer);
	auto program = parser.ParseProgram();

	ConsoleGameContext ctx;
	tsumugi::text::evaluator::Evaluator eval(ctx);

	//eval.Execute(*program);

	eval.Start(*program);
	while (!eval.IsStopRequested() && eval.Step()) {
	}

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);

	return 0;
}

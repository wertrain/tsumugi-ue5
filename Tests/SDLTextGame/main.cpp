#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include "SDLGameContext.h"
#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Text/Lexer/TextLexer.h"
#include "TsumugiEngine/Text/Parser/TextParser.h"
#include "TsumugiEngine/Text/Lexer/TextToken.h"
#include "TsumugiEngine/Text/Evaluator/TextEvaluator.h"
#include "TsumugiEngine/Text/AST/TextProgram.h"

int main(int argc, char** argv)
{
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
        *start|スタート
        [cm]
        どうしようか[r]
        [link target=*select1]選択肢１[endlink][r]
        [link target=*select2]選択肢２[endlink][r]
        [link target=*select3]選択肢３[endlink][r]

        [s]

        *select1
        [cm]
        選択肢１が選択されました。[l]
        [s]

        *select2
        [cm]
        選択肢２が選択されました。[l]
        [s]

        *select3
        [cm]
        選択肢３が選択されました。[l]
        [s]
    )";

    sample = LR"(
        ;---------------------------------------
        ; 画像とレイヤの基本
        ;---------------------------------------

        [image storage="sample\\image\\bg_room.png" layer="base"]
        [image storage="sample\\image\\char_idle.png" layer=0 left=200 top=100 visible=true opacity=255]

        「ここは主人公の部屋だ。」

        [s]
    )";

    tsumugi::text::lexer::Lexer lexer(sample.c_str());
    tsumugi::text::parser::Parser parser(&lexer);
    auto program = parser.ParseProgram();

    SDLGameContext context;
    tsumugi::text::evaluator::Evaluator eval(context);
    eval.Start(*program);

    bool quit = false;
    SDL_Event e;
    Uint64 lastTime = SDL_GetTicks();

    bool isScriptFinished = false;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) quit = true;
            context.HandleEvent(e);
        }

        Uint64 currentTime = SDL_GetTicks();
        float dt = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // スクリプトが終了しておらず、コンテキストが待機を要求していない時だけ進める
        if (!isScriptFinished) {
            bool hasNext = eval.Step();
            if (!hasNext) {
                isScriptFinished = true; // スクリプトの終端に達したら安全に Step を停止
            }
        }

        context.Update(dt);
        context.Render();

        SDL_Delay(16);
    }

    return 0;
}

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>

int main(int argc, char** argv)
{
    // SDL3 初期化
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // SDL3_ttf 初期化（成功時に true を返します）
    if (!TTF_Init()) {
        std::cerr << "TTF_Init Error: " << SDL_GetError() << std::endl; // SDL3_ttfのエラーもSDL_GetErrorで取得できます
        SDL_Quit();
        return 1;
    }

    // ウィンドウ作成（SDL3ではサイズ指定を省く）
    SDL_Window* window = SDL_CreateWindow("Hello SDL3 + SDL3_ttf", 800, 600, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // レンダラー作成（第2引数はnullptrで自動選択）
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // フォント読み込み（実行ファイルと同じフォルダ、または適切なパスに置いておく）
    //TTF_Font* font = TTF_OpenFont("NotoSans-Regular.ttf", 48);
    TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\meiryo.ttc", 48);
    if (!font) {
        std::cerr << "Font Load Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // テキスト → Surface （SDL3_ttfでは第3引数に文字数「0」を追加）
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, "Hello World!", 0, white);
    if (!surface) {
        std::cerr << "RenderText Error: " << SDL_GetError() << std::endl;
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Surface → Texture
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (!texture) {
        std::cerr << "CreateTexture Error: " << SDL_GetError() << std::endl;
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // テクスチャのサイズを取得して描画範囲(dst)を決める
    float w, h;
    SDL_GetTextureSize(texture, &w, &h);
    SDL_FRect dst = { 100, 100, w, h };

    // メインループ
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        // 画面クリア（黒）
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // 文字（テクスチャ）を描画
        SDL_RenderTexture(renderer, texture, nullptr, &dst);

        // 画面更新
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // 後片付け
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}

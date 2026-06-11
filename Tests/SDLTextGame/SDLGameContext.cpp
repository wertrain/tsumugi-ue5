#include "SDLGameContext.h"
#include <iostream>

SDLGameContext::SDLGameContext() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    }
    if (!TTF_Init()) {
        std::cerr << "TTF_Init Error: " << SDL_GetError() << std::endl;
    }

    window_ = SDL_CreateWindow("Tsumugi Novel Engine", 800, 600, 0);
    renderer_ = SDL_CreateRenderer(window_, nullptr);

    font_ = TTF_OpenFont("C:\\Windows\\Fonts\\meiryo.ttc", 28);
    if (!font_) {
        std::cerr << "Font Load Error: " << SDL_GetError() << std::endl;
    }
}

SDLGameContext::~SDLGameContext() {
    if (font_) TTF_CloseFont(font_);
    if (renderer_) SDL_DestroyRenderer(renderer_);
    if (window_) SDL_DestroyWindow(window_);
    TTF_Quit();
    SDL_Quit();
}

std::string SDLGameContext::WStringToUTF8(const std::wstring& wstr) {
    char* utf8 = SDL_iconv_string("UTF-8", "UTF-16LE",
        (const char*)wstr.c_str(),
        (wstr.size() + 1) * sizeof(wchar_t));

    if (!utf8) return "";
    std::string result = utf8;
    SDL_free(utf8);
    return result;
}

size_t SDLGameContext::NextUTF8CharSize(const char* p) {
    if (!p || *p == '\0') return 0;
    if (*p == '\n' || *p == '\r' || *p == '\t') return 1;

    const char* ptr = p;
    size_t remaining = SDL_strlen(p);
    SDL_StepUTF8(&ptr, &remaining);

    size_t size = (size_t)(ptr - p);
    return (size > 0) ? size : 1;
}

void SDLGameContext::ClearText() {
    currentText_.clear();
    visibleText_.clear();
    currentIndex_ = 0;
    textTimer_ = 0.0f;
    mode_ = Mode::Idle;
    waiting_ = false;
    lastProcessedText_ = ""; // 履歴リセット
}

void SDLGameContext::ShowText(const tstring& text) {
    std::string utf8 = WStringToUTF8(text);

    // 💡【究極の修正ポイント】
    // Evaluator が pc_ を進められずに、今さっき表示し終わったテキストと
    // 「完全に同じ文字列」を再実行して突っ込んできた場合：
    if (lastProcessedText_ == utf8 && currentIndex_ >= currentText_.size()) {
        // 「もう私は表示し終わっています！」という合図として、
        // waiting_ をあえて false にします。
        // これにより、Evaluator::Step() 末尾の if (context_.IsWaiting()) を
        // すり抜けさせ、pc_++ を強制的に実行させて次の命令へ進めます！
        waiting_ = false;
        return;
    }

    // 💡 本当に新しいテキスト（または次の細切れテキスト）が来た場合のみ結合・再生する
    currentText_ += utf8;
    lastProcessedText_ = utf8; // 最後に処理した生の文字列を記憶

    mode_ = Mode::ShowingText;
    waiting_ = true; // 新規再生が始まるので、このフレーム内での先読みをブロック
}

void SDLGameContext::NewLine() {
    currentText_ += "\n";
    mode_ = Mode::ShowingText;
    waiting_ = true;
}

void SDLGameContext::PageBreak() {
    mode_ = Mode::WaitingForClick;
    waiting_ = true;
}

void SDLGameContext::WaitForInput() {
    mode_ = Mode::WaitingForClick;
    waiting_ = true;
}

void SDLGameContext::ApplyFontState(const tsumugi::text::context::FontState& state) {
    fontState_ = state;
    try {
        size_t offset = (state.color.substr(0, 2) == L"0x") ? 2 : 1;
        int r = std::stoi(state.color.substr(offset, 2), nullptr, 16);
        int g = std::stoi(state.color.substr(offset + 2, 2), nullptr, 16);
        int b = std::stoi(state.color.substr(offset + 4, 2), nullptr, 16);
        currentColor_ = { (Uint8)r, (Uint8)g, (Uint8)b, 255 };
    }
    catch (...) {
        currentColor_ = { 255, 255, 255, 255 };
    }
}

void SDLGameContext::ApplyDelayState(const tsumugi::text::context::DelayState& state) {
    delayState_ = state;
}

void SDLGameContext::Wait(int ms) {
    waitTimer_ = (float)ms / 1000.0f;
    waiting_ = true;
    mode_ = Mode::WaitingTimer;
}

void SDLGameContext::HandleEvent(const SDL_Event& e) {
    if (mode_ == Mode::WaitingForClick) {
        if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN || e.type == SDL_EVENT_KEY_DOWN) {

            // クリックされたら画面バッファを完全クリアして、完全な自由状態（Idle）に戻す
            currentText_.clear();
            visibleText_.clear();
            currentIndex_ = 0;
            textTimer_ = 0.0f;
            lastProcessedText_ = ""; // 履歴も消去

            waiting_ = false;
            mode_ = Mode::Idle;
        }
    }
}

void SDLGameContext::Update(float dt) {
    switch (mode_) {
    case Mode::ShowingText: {
        // 文字送りアニメーション
        if (currentIndex_ < currentText_.size()) {
            textTimer_ += dt;

            float interval = 0.03f;
            if (delayState_.mode == tsumugi::text::context::DelayState::Mode::Fixed) {
                interval = (float)delayState_.fixedSpeedMs / 1000.0f;
            }
            else if (delayState_.mode == tsumugi::text::context::DelayState::Mode::Nowait) {
                interval = 0.0f;
            }

            while (textTimer_ >= interval || interval == 0.0f) {
                if (interval > 0.0f) {
                    textTimer_ -= interval;
                }
                else {
                    textTimer_ = 0.0f;
                }

                size_t charSize = NextUTF8CharSize(currentText_.c_str() + currentIndex_);
                if (charSize > 0) {
                    visibleText_ += currentText_.substr(currentIndex_, charSize);
                    currentIndex_ += charSize;
                }
                else {
                    currentIndex_++;
                }

                if (currentIndex_ >= currentText_.size()) break;
                if (interval == 0.0f) break;
            }
        }

        // 💡 文字送り完了時
        if (currentIndex_ >= currentText_.size()) {
            // 文字送りは終わったが、Evaluatorが次のStep()を叩きに来るまでは、
            // モードを Idle に落とさない（ShowingTextのまま維持）。
            // ただし、main.cppがeval.Step()を呼び出せるように、外向けのウェイトフラグだけを下ろす。
            waiting_ = false;
        }
        break;
    }

    case Mode::WaitingTimer: {
        waitTimer_ -= dt;
        if (waitTimer_ <= 0.0f) {
            waiting_ = false;
            mode_ = Mode::Idle;
        }
        break;
    }

    default:
        break;
    }
}

void SDLGameContext::Render() {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    if (!visibleText_.empty() && font_) {
        SDL_Surface* surface = TTF_RenderText_Solid(font_, visibleText_.c_str(), visibleText_.length(), currentColor_);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
            if (texture) {
                float w, h;
                SDL_GetTextureSize(texture, &w, &h);
                SDL_FRect dst = { (float)cursorX_, (float)cursorY_, (float)w, (float)h };
                SDL_RenderTexture(renderer_, texture, nullptr, &dst);
                SDL_DestroyTexture(texture);
            }
            SDL_DestroySurface(surface);
        }
    }

    if (mode_ == Mode::WaitingForClick) {
        SDL_SetRenderDrawColor(renderer_, 0, 170, 255, 255);
        SDL_FRect sign = { 730.0f, 520.0f, 20.0f, 20.0f };
        SDL_RenderFillRect(renderer_, &sign);
    }

    SDL_RenderPresent(renderer_);
}

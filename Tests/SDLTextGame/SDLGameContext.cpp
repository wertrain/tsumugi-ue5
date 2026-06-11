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
}

void SDLGameContext::ShowText(const tstring& text) {
    std::string utf8 = WStringToUTF8(text);

    currentText_ += utf8;
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
    // 背景クリア（黒）
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    if (!visibleText_.empty() && font_) {
        // ウィンドウの現在のサイズを動的に取得する
        int winW = 800;
        int winH = 600;
        SDL_GetWindowSize(window_, &winW, &winH);
        // 右側にも左側と同じだけの余白（マージン）を持たせる
        int paddingRight = cursorX_;
        int wrapWidth = winW - cursorX_ - paddingRight;

        // もしウィンドウが極端に狭くなった場合の安全弁
        if (wrapWidth < 100) wrapWidth = 100;

        SDL_Surface* surface = TTF_RenderText_Solid_Wrapped(
            font_,
            visibleText_.c_str(),
            visibleText_.length(),
            currentColor_,
            wrapWidth
        );

        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
            if (texture) {
                float w, h;
                SDL_GetTextureSize(texture, &w, &h);

                // cursorX_, cursorY_ を基準に描画
                SDL_FRect dst = { (float)cursorX_, (float)cursorY_, w, h };
                SDL_RenderTexture(renderer_, texture, nullptr, &dst);
                SDL_DestroyTexture(texture);
            }
            SDL_DestroySurface(surface);
        }
    }

    // [p] タグによるクリック待ち記号の描画
    if (mode_ == Mode::WaitingForClick) {
        SDL_SetRenderDrawColor(renderer_, 0, 170, 255, 255);

        int winW = 800;
        int winH = 600;
        SDL_GetWindowSize(window_, &winW, &winH);

        float signSize = 20.0f;
        float marginRight = 50.0f;
        float marginBottom = 50.0f;
        float signX = (float)winW - signSize - marginRight;
        float signY = (float)winH - signSize - marginBottom;

        SDL_FRect sign = { signX, signY, signSize, signSize };
        SDL_RenderFillRect(renderer_, &sign);
    }

    SDL_RenderPresent(renderer_);
}

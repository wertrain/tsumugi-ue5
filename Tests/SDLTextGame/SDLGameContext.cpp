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

void SDLGameContext::AddChoice(const tstring& text, const tstring& target) {

    // 1. テキストを UTF-8 に変換してサーフェスを作成
    std::string utf8Text = WStringToUTF8(text);

    // 選択肢用の色（例として少し明るい水色。お好みで fontState_ から取ってもOKです）
    SDL_Color choiceColor = { 0, 200, 255, 255 };

    SDL_Surface* surface = TTF_RenderText_Solid(font_, utf8Text.c_str(), utf8Text.length(), choiceColor);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    float w = (float)surface->w;
    float h = (float)surface->h;
    SDL_DestroySurface(surface);

    if (!texture) return;

    // 2. 選択肢の配置座標（Rect）を計算する
    // 今回はシンプルに「画面中央」に上から順に並べるレイアウトにします
    int winW = 800;
    SDL_GetWindowSize(window_, &winW, nullptr);

    float posX = (winW - w) / 2.0f; // 中央揃え

    // 既存の選択肢の数に応じて Y 座標を下にずらしていく
    // 画面中央やや下方（y=300）から、50ピクセル間隔で並べる例
    float posY = 300.0f + (choices_.size() * 60.0f);

    SDL_FRect rect = { posX, posY, w, h };

    // 3. 即時クリック可能な選択肢としてリストに登録
    choices_.push_back({ text, target, rect, texture });
}

void SDLGameContext::ClearChoices() {

    // テクスチャのメモリリークを防ぐために破棄
    for (auto& choice : choices_) {
        if (choice.texture) {
            SDL_DestroyTexture(choice.texture);
        }
    }
    choices_.clear();
    selectedTarget_ = std::nullopt;
}

std::optional<tstring> SDLGameContext::PollChoice() { 

    // 選択されたターゲットがあれば、それを引っこ抜いて返す
    if (selectedTarget_.has_value()) {
        auto target = selectedTarget_;

        // ジャンプが確定するので、現在の選択肢はすべてクリアする（KAG仕様）
        ClearChoices();
        return target;
    }
    return std::nullopt;
}

void SDLGameContext::HandleEvent(const SDL_Event& e) {

    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && !choices_.empty()) {
        float mouseX = e.button.x;
        float mouseY = e.button.y;
        SDL_FPoint mousePos = { mouseX, mouseY };

        for (const auto& choice : choices_) {
            if (mousePos.x >= choice.rect.x && mousePos.x <= (choice.rect.x + choice.rect.w) &&
                mousePos.y >= choice.rect.y && mousePos.y <= (choice.rect.y + choice.rect.h)) {
                // ヒットしたターゲットを記録（次の PollChoice で Evaluator に回収される）
                selectedTarget_ = choice.target;

                // 選択肢が選ばれたので、通常の画面クリック待ち（WaitingForClick）などの
                // 他のウエイト状態も強制解除して Idle に戻す
                waiting_ = false;
                mode_ = Mode::Idle;
                return; // 処理確定のため終了
            }
        }
    }

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

    // ★ 追加：選択肢（ボタン）の描画処理
    for (const auto& choice : choices_) {
        if (choice.texture) {
            // AddChoice 時に計算した rect の場所にテクスチャを描画
            SDL_RenderTexture(renderer_, choice.texture, nullptr, &choice.rect);

            // デバッグやUIの見た目用に、うっすら枠線を描くとボタンらしくなります（お好みで）
            SDL_SetRenderDrawColor(renderer_, 0, 200, 255, 100);
            SDL_RenderRect(renderer_, &choice.rect);
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

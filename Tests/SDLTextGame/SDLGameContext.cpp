#include "SDLGameContext.h"
#include <SDL3_image/SDL_image.h>
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

    fgLayers.resize(16);
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

void SDLGameContext::SetLayer(const tsumugi::text::context::LayerParams& params) {

    // --- 1. 対象レイヤを決定 ---
    Layer* layer = nullptr;

    if (params.isBase) {
        layer = &baseLayer;
    }
    else {
        if (params.layerIndex >= fgLayers.size()) {
            size_t oldSize = fgLayers.size();
            fgLayers.resize(params.layerIndex + 1);

            for (size_t i = oldSize; i < fgLayers.size(); ++i) {
                fgLayers[i].texture = nullptr;
                fgLayers[i].visible = false;
                fgLayers[i].x = 0;
                fgLayers[i].y = 0;
                fgLayers[i].opacity = 255;
            }
        }
        layer = &fgLayers[params.layerIndex];
    }

    // --- 2. 画像読み込み ---
    if (!params.storage.empty()) {
        std::string path = tsumugi::type::convert::TStringToString(params.storage);

        SDL_Surface* surf = IMG_Load(path.c_str());
        if (!surf) {
            SDL_Log("Failed to load image: %s", path.c_str());
        }
        else {
            // 💡 修正：renderer_ (アンダースコアあり) に統一
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer_, surf);
            SDL_DestroySurface(surf);

            if (!tex) {
                SDL_Log("Failed to create texture: %s", path.c_str());
            }
            else {
                if (layer->texture) {
                    SDL_DestroyTexture(layer->texture);
                }
                layer->texture = tex;
                SDL_SetTextureBlendMode(layer->texture, SDL_BLENDMODE_BLEND);
            }
        }
    }

    // --- 3. 各種パラメータの同期 ---
    if (params.hasVisible) layer->visible = params.visible;
    if (params.hasLeft)    layer->x = params.left;
    if (params.hasTop)     layer->y = params.top;
    if (params.hasOpacity) layer->opacity = params.opacity;

    if (layer->texture) {
        SDL_SetTextureAlphaMod(layer->texture, layer->opacity);
    }
}

void SDLGameContext::AddChoice(const tstring& text, const tstring& target) {

    std::string utf8Text = WStringToUTF8(text);
    SDL_Color choiceColor = { 0, 200, 255, 255 }; // 選択肢と分かりやすい色

    SDL_Surface* surface = TTF_RenderText_Solid(font_, utf8Text.c_str(), utf8Text.length(), choiceColor);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    float w = (float)surface->w;
    float h = (float)surface->h;
    SDL_DestroySurface(surface);

    if (!texture) return;

    // 💡 座標はここでは 0 にしておき、Render 時のテキストの高さに合わせる
    SDL_FRect rect = { 0.0f, 0.0f, w, h };
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

        // 文字送り完了時
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
    // --- 1. 画面のクリア ---
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    // 【背景レイヤの描画
    if (baseLayer.visible && baseLayer.texture) {
        float w, h;
        SDL_GetTextureSize(baseLayer.texture, &w, &h);
        SDL_FRect dst = { (float)baseLayer.x, (float)baseLayer.y, w, h };
        SDL_RenderTexture(renderer_, baseLayer.texture, nullptr, &dst);
    }

    // 前景レイヤ（立ち絵など）
    for (const auto& layer : fgLayers) {
        if (layer.visible && layer.texture) {
            float w, h;
            SDL_GetTextureSize(layer.texture, &w, &h);
            SDL_FRect dst = { (float)layer.x, (float)layer.y, w, h };
            SDL_RenderTexture(renderer_, layer.texture, nullptr, &dst);
        }
    }

    // --- 2. テキスト・UIレイヤの描画（最手前） ---
    if (!font_) return;

    int winW = 800;
    SDL_GetWindowSize(window_, &winW, nullptr);
    int paddingRight = cursorX_;
    int wrapWidth = winW - cursorX_ - paddingRight;
    if (wrapWidth < 100) wrapWidth = 100;

    if (!visibleText_.empty()) {
        SDL_Surface* surface = TTF_RenderText_Solid_Wrapped(
            font_, visibleText_.c_str(), visibleText_.length(), currentColor_, wrapWidth
        );
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
            if (texture) {
                float w, h;
                SDL_GetTextureSize(texture, &w, &h);
                SDL_FRect dst = { (float)cursorX_, (float)cursorY_, w, h };

                SDL_RenderTexture(renderer_, texture, nullptr, &dst);
                SDL_DestroyTexture(texture);
            }
            SDL_DestroySurface(surface);
        }
    }

    // --- 3. 選択肢の幾何計算とアタリ判定枠の描画 ---
    if (currentIndex_ >= currentText_.size()) {
        int fontHeight = TTF_GetFontHeight(font_);
        std::string cumulativeText = currentText_;

        for (size_t i = choices_.size(); i > 0; --i) {
            size_t idx = i - 1;
            auto& choice = choices_[idx];
            std::string choiceUtf8 = WStringToUTF8(choice.text);

            SDL_Surface* afterSurf = TTF_RenderText_Solid_Wrapped(
                font_, cumulativeText.c_str(), cumulativeText.length(), currentColor_, wrapWidth
            );

            if (cumulativeText.length() >= choiceUtf8.length()) {
                if (!cumulativeText.empty() && cumulativeText.back() == '\n') {
                    cumulativeText.pop_back();
                }
                if (cumulativeText.length() >= choiceUtf8.length()) {
                    cumulativeText = cumulativeText.substr(0, cumulativeText.length() - choiceUtf8.length());
                }
            }

            SDL_Surface* beforeSurf = TTF_RenderText_Solid_Wrapped(
                font_, cumulativeText.c_str(), cumulativeText.length(), currentColor_, wrapWidth
            );

            if (afterSurf) {
                if (beforeSurf) {
                    if (afterSurf->h > beforeSurf->h) {
                        choice.rect.x = (float)cursorX_;
                        choice.rect.y = (float)cursorY_ + beforeSurf->h;
                    }
                    else {
                        choice.rect.x = (float)cursorX_ + beforeSurf->w;
                        choice.rect.y = (float)cursorY_ + (beforeSurf->h - fontHeight);
                    }
                }
                else {
                    choice.rect.x = (float)cursorX_;
                    choice.rect.y = (float)cursorY_;
                }

                SDL_Surface* singleChoiceSurf = TTF_RenderText_Solid(
                    font_, choiceUtf8.c_str(), choiceUtf8.length(), currentColor_
                );
                if (singleChoiceSurf) {
                    choice.rect.w = (float)singleChoiceSurf->w;
                    choice.rect.h = (float)singleChoiceSurf->h;
                    SDL_DestroySurface(singleChoiceSurf);
                }

                SDL_DestroySurface(afterSurf);
            }
            if (beforeSurf) SDL_DestroySurface(beforeSurf);

            // 選択肢のバウンディングボックス（判定枠）を可視化
            SDL_SetRenderDrawColor(renderer_, 0, 200, 255, 255);
            SDL_RenderRect(renderer_, &choice.rect);
        }
    }

    // --- 4. バックバッファを画面に転送 ---
    SDL_RenderPresent(renderer_);
}

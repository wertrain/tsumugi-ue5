#pragma once
#include "TsumugiEngine/Text/Context/IGameContext.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

struct Layer {
    SDL_Texture* texture = nullptr;
    bool visible = true;
    int x = 0;
    int y = 0;
    int opacity = 255;
};

class SDLGameContext : public tsumugi::text::context::IGameContext {
public:
    SDLGameContext();
    ~SDLGameContext();

    // --- IGameContext API ---
    void ClearText() override;
    void ShowText(const tstring& text) override;
    void NewLine() override;
    void PageBreak() override;
    void WaitForInput() override;

    tsumugi::text::context::FontState& GetFontState() override { return fontState_; }
    void ApplyFontState(const tsumugi::text::context::FontState& state) override;

    tsumugi::text::context::DelayState& GetDelayState() override { return delayState_; }
    void ApplyDelayState(const tsumugi::text::context::DelayState& state) override;

    void Wait(int ms) override;
    void ShakeScreen(int strength, int time) override {}
    void FadeIn(int time) override {}
    void FadeOut(int time) override {}

    void SetLayer(const tsumugi::text::context::LayerParams& params) override;
    void ShowImage(const tstring& layer, const tstring& path) override {}
    void HideImage(const tstring& layer) override {}
    void MoveImage(const tstring& layer, int x, int y, int time) override {}
    void ClearLayer(const tstring& layer) override {}

    void PlayBGM(const tstring& path, int loop = -1) override {}
    void StopBGM(int time = 0) override {}
    void PlaySE(const tstring& path) override {}
    void StopSE(const tstring& path) override {}

    void Save(int slot) override {}
    void Load(int slot) override {}
    void SetUserFont(const tstring& fontName) override {}
    void SetUserTextSpeed(int ms) override {}

    bool IsWaiting() const override { return waiting_; }

    void AddChoice(const tstring& text, const tstring& target) override;
    void ClearChoices() override;
    std::optional<tstring> PollChoice() override;

    void HandleEvent(const SDL_Event& e);
    void Update(float dt);
    void RebuildLayout();
    void Render();

private:
    enum class Mode {
        Idle,
        ShowingText,
        WaitingForClick,
        WaitingTimer
    };

    struct SDLChoice {
        tstring text;
        tstring target;
        SDL_FRect rect;        // マウスクリック判定用の画面座標
        SDL_Texture* texture;  // 生成したテキストテクスチャ（キャッシュ用）
    };

    Mode mode_ = Mode::Idle;

    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    TTF_Font* font_ = nullptr;

    tsumugi::text::context::FontState fontState_;
    tsumugi::text::context::DelayState delayState_;

    SDL_Color currentColor_ = { 255, 255, 255, 255 };

    std::string currentText_;
    std::string visibleText_;
    size_t currentIndex_ = 0;
    float textTimer_ = 0.0f;

    float waitTimer_ = 0.0f;
    bool waiting_ = false;               // スクリプト停止フラグ

    int cursorX_ = 50;
    int cursorY_ = 50;

    std::vector<SDLChoice> choices_;
    std::optional<tstring> selectedTarget_ = std::nullopt;
    float penX_ = 0.0f;
    float penY_ = 0.0f;
    int lineHeight_ = 0;

    Layer baseLayer;
    std::vector<Layer> fgLayers;

    std::string WStringToUTF8(const std::wstring& wstr);
    size_t NextUTF8CharSize(const char* p);
};
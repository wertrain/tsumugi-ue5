#pragma once

#include "Foundation/Types.h"
#include "Text/Context/FontState.h"
#include "Text/Context/DelayState.h"
#include "Text/Context/LayerParams.h"
#include <optional>

namespace tsumugi::text::context {

class IGameContext {
public:
    virtual ~IGameContext() = default;

    virtual void ClearText() = 0;
    virtual void ShowText(const tstring& text) = 0;
    virtual void NewLine() = 0;
    virtual void PageBreak() = 0;
    virtual void WaitForInput() = 0;

    // --- フォント ---
    virtual FontState& GetFontState() = 0;
    virtual void ApplyFontState(const FontState& state) = 0;

    // --- 文字送り速度 ---
    virtual DelayState& GetDelayState() = 0;
    virtual void ApplyDelayState(const DelayState& state) = 0;

    // --- 演出 ---
    virtual void Wait(int ms) = 0;
    virtual void ShakeScreen(int strength, int time) = 0;
    virtual void FadeIn(int time) = 0;
    virtual void FadeOut(int time) = 0;

    // --- 画像・レイヤー ---
    virtual void SetLayer(const LayerParams& params) = 0;
    virtual void ShowImage(const tstring& layer, const tstring& path) = 0;
    virtual void HideImage(const tstring& layer) = 0;
    virtual void MoveImage(const tstring& layer, int x, int y, int time) = 0;
    virtual void ClearLayer(const tstring& layer) = 0;

    // --- サウンド ---
    virtual void PlayBGM(const tstring& path, int loop = -1) = 0;
    virtual void StopBGM(int time = 0) = 0;
    virtual void PlaySE(const tstring& path) = 0;
    virtual void StopSE(const tstring& path) = 0;

    // --- システム ---
    virtual void Save(int slot) = 0;
    virtual void Load(int slot) = 0;
    virtual void SetUserFont(const tstring& fontName) = 0;
    virtual void SetUserTextSpeed(int ms) = 0;

    // --- 実行制御 ---
    virtual bool IsWaiting() const = 0;

    // --- 選択肢 ---
    virtual void AddChoice(const tstring& text, const tstring& target) = 0;
    virtual void ClearChoices() = 0;
    virtual std::optional<tstring> PollChoice() = 0;
};

}

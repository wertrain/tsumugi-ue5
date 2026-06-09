#pragma once
#include "Text/Context/IGameContext.h"
#include <iostream>
#include <thread>

class ConsoleGameContext : public tsumugi::text::context::IGameContext {
public:
    ConsoleGameContext();

    // --- テキスト表示 ---
    void ClearText() override;
    void ShowText(const tstring& text) override;
    void NewLine() override;
    void PageBreak() override;
    void WaitForClick() override;

    // --- フォント ---
    tsumugi::text::context::FontState& GetFontState() override { return fontState_; }
    void ApplyFontState(const tsumugi::text::context::FontState& state) override;

    // --- 文字送り速度 ---
    tsumugi::text::context::DelayState& GetDelayState() override { return delayState_; }
    void ApplyDelayState(const tsumugi::text::context::DelayState& state) override;

    // --- 演出 ---
    void Wait(int ms) override;
    void ShakeScreen(int strength, int time) override {}
    void FadeIn(int time) override {}
    void FadeOut(int time) override {}

    // --- 画像・レイヤー ---
    void ShowImage(const tstring& layer, const tstring& path) override {}
    void HideImage(const tstring& layer) override {}
    void MoveImage(const tstring& layer, int x, int y, int time) override {}
    void ClearLayer(const tstring& layer) override {}

    // --- サウンド ---
    void PlayBGM(const tstring& path, int loop = -1) override {}
    void StopBGM(int time = 0) override {}
    void PlaySE(const tstring& path) override {}
    void StopSE(const tstring& path) override {}

    // --- システム ---
    void Save(int slot) override {}
    void Load(int slot) override {}
    void SetUserFont(const tstring& fontName) override {}
    void SetUserTextSpeed(int ms) override {}

    // --- 実行制御 ---
    virtual bool IsWaiting() const override;
private:
    tsumugi::text::context::FontState fontState_;
    tsumugi::text::context::DelayState delayState_;

    void ApplyConsoleColor(const tstring& hexColor);
};

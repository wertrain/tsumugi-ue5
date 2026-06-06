#include "ConsoleGameContext.h"
#include "Script/Objects/Environment.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>

ConsoleGameContext::ConsoleGameContext()
    : fontState_()
    , delayState_() {

    // 1. Windowsコンソールの設定
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // 2. 仮想ターミナル（エスケープシーケンス）の有効化
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }

    // 3. ロケールをシステム既定（動いているコードと同じ設定）にする
    std::setlocale(LC_ALL, "");
}

// --- テキスト表示 ---
void ConsoleGameContext::ClearText() {

    std::wcout << L"\x1b[2J\x1b[H"; // 画面クリア（ページ送り）
    //std::wcout << L"\x1b[3J\x1b[2J\x1b[H" << std::flush;
}

void ConsoleGameContext::ShowText(const tstring& text) {

    // 色を適用
    ApplyConsoleColor(fontState_.color);

    // 文字送り速度に応じて 1 文字ずつ表示
    for (auto ch : text) {
        std::wcout << ch << std::flush;

        if (delayState_.mode == tsumugi::text::context::DelayState::Mode::Fixed) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delayState_.fixedSpeedMs));
        }
        else if (delayState_.mode == tsumugi::text::context::DelayState::Mode::User) {
            std::this_thread::sleep_for(std::chrono::milliseconds(30)); // 仮のユーザー速度
        }
        // nowait は何もしない
    }
}

void ConsoleGameContext::NewLine() {

    std::wcout << L"\n";
}

void ConsoleGameContext::PageBreak() {

    std::wcout << L"\n--- Press Enter to continue ---";
    std::wcin.get();
    std::wcout << L"\n";
}

void ConsoleGameContext::WaitForClick() {

    std::wcout << L"\n[Click to continue]";
    std::wcin.get();
}

// --- フォント ---
void ConsoleGameContext::ApplyFontState(const tsumugi::text::context::FontState& state) {

    // コンソールでは色のみ対応
    ApplyConsoleColor(state.color);
}

// --- 文字送り速度 ---
void ConsoleGameContext::ApplyDelayState(const tsumugi::text::context::DelayState& state) {

    // 特に何もしない（ShowText が参照する）
}

// --- 演出 ---
void ConsoleGameContext::Wait(int ms) {

    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// --- 色変換 ---
void ConsoleGameContext::ApplyConsoleColor(const tstring& hexColor) {

    if (hexColor.size() != 8 || hexColor[0] != '0' || hexColor[1] != 'x') {
        std::wcout << L"\x1b[0m"; // ⭕ wstringリテラル
        return;
    }

    int r = std::stoi(hexColor.substr(2, 2), nullptr, 16);
    int g = std::stoi(hexColor.substr(4, 2), nullptr, 16);
    int b = std::stoi(hexColor.substr(6, 2), nullptr, 16);

    std::wcout << L"\x1b[38;2;" << r << L";" << g << L";" << b << L"m";
}


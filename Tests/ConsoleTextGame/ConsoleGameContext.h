#pragma once
#include "Text/Context/IGameContext.h"
#include <iostream>
#include <thread>

class ConsoleGameContext : public tsumugi::text::context::IGameContext {
public:
    void Wait(int ms) override {
        std::wcout << L"[WAIT] " << ms << L"ms\n";
    }

    void ClearText() override {
        std::wcout << L"[CLEAR]\n";
    }

    void ShowText(const tstring& text) override {
        std::wcout << text << L"\n";
    }
};

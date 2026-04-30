#pragma once

#include "Foundation/Types.h"
#include "Log/LogConsole.h"
#include <array>
#include <vector>
#include <cstdarg>
#include <sstream>

namespace tsumugi::log {

class DefaultConsole : public ILogConsole {
public:
    DefaultConsole() {}
    void WriteLine(const tlogchar* text);
};

class TextLogger {
public:
    /// <summary>
    /// ログのカテゴリ
    /// </summary>
    enum class Categories
    {
        Default,
        Information,
        Warning,
        Error,
        Num
    };

    /// <summary>
    /// 履歴
    /// </summary>
    struct History
    {
        Categories category;
        tlogstring message;

        History(const Categories& category, const tlogstring& message)
            : category(category)
            , message(message) {
        }
    };

public:
    TextLogger() :
        histories_(),
        console_(&defaultConsole_)
    {}
    virtual ~TextLogger() {}
    
    template<typename T>
    TextLogger& operator<<(const T& value) {

        std::stringstream stream;
        (stream << value);
        Log(Categories::Default, tlogstring(stream.str()));
        return *this;
    }

    /// <summary>
    /// ログを表示
    /// </summary>
    /// <param name="category">カテゴリ</param>
    /// <param name="format">表示するログの書式</param>
    template<typename... Args>
    void Log(const Categories& category, Args... args) {

        std::stringstream stream;
        (stream << ... << args);
        Log_(category, tlogstring(stream.str()));
    }

    /// <summary>
    /// ログを記録
    /// </summary>
    /// <param name="category">カテゴリ</param>
    /// <param name="format">記録するログの書式</param>
    template<typename... Args>
    void Logging(const Categories& category, Args... args) {

        std::stringstream stream;
        (stream << ... << args);
        Logging_(category, tlogstring(stream.str()));
    }

    /// <summary>
    /// ログ履歴を取得
    /// </summary>
    /// <returns></returns>
    const auto& GetHistories() const { return histories_; }
    
    /// <summary>
    /// 指定したカテゴリのログ履歴を取得
    /// </summary>
    /// <param name="c"></param>
    /// <returns></returns>
    const std::vector<History>& GetHistory(Categories c) const {
        return histories_[static_cast<size_t>(c)];
    }

    /// <summary>
    /// 指定したカテゴリのログが存在するかどうかを取得
    /// </summary>
    /// <param name="c"></param>
    /// <returns></returns>
    bool HasLog(Categories c) const {
        return !histories_[static_cast<size_t>(c)].empty();
    }

    /// <summary>
    /// いずれかのログが存在するかどうかを取得
    /// </summary>
    /// <returns></returns>
    bool HasAnyLog() const {
        for (const auto& vec : histories_) {
            if (!vec.empty()) return true;
        }
        return false;
    }

    const auto& GetLogConsole() const { return console_; }
    void SetLogConsole(ILogConsole* console) { console_ = console; }

private:
    /// <summary>
    /// ログを表示
    /// </summary>
    /// <param name="category">カテゴリ</param>
    /// <param name="message">表示するログ</param>
    virtual void Log_(const Categories& category, const tlogstring& message) {

        Logging(category, message);
        if (console_) {
            console_->WriteLine(message.c_str());
        }
    }

    /// <summary>
    /// ログを記録
    /// </summary>
    /// <param name="category">カテゴリ</param>
    /// <param name="message">記録するログ</param>
    virtual void Logging_(const Categories& category, const tlogstring& message) {

        histories_[(int)category].push_back(History(category, message));
    }

private:
    std::array<std::vector<History>, static_cast<size_t>(Categories::Num)> histories_;
    ILogConsole* console_;
    DefaultConsole defaultConsole_;
};

}

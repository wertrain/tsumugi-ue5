#pragma once

#include "Foundation/Types.h"
#include <array>
#include <vector>
#include <cstdarg>
#include <sstream>

namespace tsumugi::log {

using tlogchar = char;
using tlogstring = std::string;
extern const char* tlognewline;

/// <summary>
/// ���O�̏o�͑Ώ�
/// </summary>
class ILogConsole {
public:
    ILogConsole() {}
    virtual void WriteLine(const tlogchar* text) = 0;
};

class DefaultConsole : public ILogConsole {
public:
    DefaultConsole() {}
    void WriteLine(const tlogchar* text);
};

class TextLogger {
public:
    /// <summary>
    /// ���O�̃J�e�S��
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
    /// ����
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
    /// ���O��\��
    /// </summary>
    /// <param name="category">�J�e�S��</param>
    /// <param name="format">�\�����郍�O�̏���</param>
    template<typename... Args>
    void Log(const Categories& category, Args... args) {

        std::stringstream stream;
        (stream << ... << args);
        Log_(category, tlogstring(stream.str()));
    }

    /// <summary>
    /// ���O���L�^
    /// </summary>
    /// <param name="category">�J�e�S��</param>
    /// <param name="format">�L�^���郍�O�̏���</param>
    template<typename... Args>
    void Logging(const Categories& category, Args... args) {

        std::stringstream stream;
        (stream << ... << args);
        Logging_(category, tlogstring(stream.str()));
    }

    const auto& GetLogConsole() const { return console_; }
    void SetLogConsole(ILogConsole* console) { console_ = console; }

private:
    /// <summary>
    /// ���O��\��
    /// </summary>
    /// <param name="category">�J�e�S��</param>
    /// <param name="message">�\�����郍�O</param>
    virtual void Log_(const Categories& category, const tlogstring& message) {

        Logging(category, message);
        if (console_) {
            console_->WriteLine(message.c_str());
        }
    }

    /// <summary>
    /// ���O���L�^
    /// </summary>
    /// <param name="category">�J�e�S��</param>
    /// <param name="message">�L�^���郍�O</param>
    virtual void Logging_(const Categories& category, const tlogstring& message) {

        histories_[(int)category].push_back(History(category, message));
    }

private:
    std::array<std::vector<History>, static_cast<size_t>(Categories::Num)> histories_;
    ILogConsole* console_;
    DefaultConsole defaultConsole_;
};

}

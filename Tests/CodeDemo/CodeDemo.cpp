#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include <unordered_set>
#include <cwctype>
#include <clocale>
#include <algorithm>
#include <random>

// Windows ヘッダの前に定義
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include "TsumugiEngine/Script/Lexer/ScriptLexer.h"
#include "TsumugiEngine/Script/Parser/ScriptParser.h"
#include "TsumugiEngine/Script/AST/Program.h"
#include "TsumugiEngine/Script/Evaluator/Evaluator.h"
#include "TsumugiEngine/Script/Objects/Environment.h"

// 24bit ANSI 色（Visual Studio Dark テーマに近い）
static const std::wstring VS_KEYWORD = L"\x1b[38;2;86;156;214m";   // #569CD6
static const std::wstring VS_TYPE = L"\x1b[38;2;78;201;176m";   // #4EC9B0
static const std::wstring VS_METHOD = L"\x1b[38;2;220;220;170m";  // #DCDCAA
static const std::wstring VS_CONS = L"\x1b[38;2;156;220;254m";  // #9CDCFE
static const std::wstring VS_STRING = L"\x1b[38;2;206;145;120m";  // #CE9178
static const std::wstring VS_COMMENT = L"\x1b[38;2;106;153;85m";   // #6A9955
static const std::wstring VS_NUMBER = L"\x1b[38;2;181;206;168m";  // #B5CEA8
static const std::wstring VS_PUNCT = L"\x1b[38;2;212;212;212m";  // #D4D4D4
static const std::wstring VS_NORMAL = L"\x1b[38;2;212;212;212m";  // #D4D4D4
static const std::wstring RESET = L"\x1b[0m";

// 8色フォールバック（ANSI 標準色）
static const std::wstring F_KEYWORD = L"\x1b[34m"; // blue
static const std::wstring F_TYPE = L"\x1b[36m"; // cyan
static const std::wstring F_METHOD = L"\x1b[33m"; // yellow
static const std::wstring F_CONS = L"\x1b[36m"; // cyan
static const std::wstring F_STRING = L"\x1b[31m"; // red/orange
static const std::wstring F_COMMENT = L"\x1b[32m"; // green
static const std::wstring F_NUMBER = L"\x1b[32m"; // green
static const std::wstring F_PUNCT = L"\x1b[37m"; // white
static const std::wstring F_NORMAL = L"\x1b[37m"; // white

// 実行時に使う色セット（24bit が使えるかで切り替える）
struct ColorSet {
    const std::wstring* KEYWORD;
    const std::wstring* TYPE;
    const std::wstring* METHOD;
    const std::wstring* CONS;
    const std::wstring* STRING;
    const std::wstring* COMMENT;
    const std::wstring* NUMBER;
    const std::wstring* PUNCT;
    const std::wstring* NORMAL;
    const std::wstring* RESET;
};
static ColorSet colors24 = { &VS_KEYWORD, &VS_TYPE, &VS_METHOD, &VS_CONS, &VS_STRING, &VS_COMMENT, &VS_NUMBER, &VS_PUNCT, &VS_NORMAL, &RESET };
static ColorSet colors8 = { &F_KEYWORD, &F_TYPE, &F_METHOD, &F_CONS, &F_STRING, &F_COMMENT, &F_NUMBER, &F_PUNCT, &F_NORMAL, &RESET };

// Try to enable ANSI and return whether we will use 24bit colors (true) or fallback to 8 colors (false)
bool TryEnableAnsiAndDecide24bit()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return false;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return false;
    // Enable virtual terminal processing
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode)) {
        return false;
    }
    // Heuristic: modern Windows 10+ supports 24bit; assume true.
    // If you need stricter detection, check OS version or terminal capabilities.
    return true;
}

// キーワード集合
std::unordered_set<std::wstring> keywords = {
    L"let", L"function", L"if", L"else", L"return", L"true", L"false", L"class", L"break", L"continue", L"while", L"for", L"null"
};

// 前方/後方にあるトークンを簡易チェックするユーティリティ
static bool PrevIsKeyword(const std::wstring& code, size_t start, const std::wstring& kw)
{
    if (start == 0) return false;
    size_t i = (start == 0) ? 0 : start - 1;
    while (i < code.size() && iswspace(code[i])) {
        if (i == 0) { i = (size_t)-1; break; }
        --i;
    }
    if (i == (size_t)-1) return false;
    if (i + 1 < kw.size()) return false;
    size_t pos = (i + 1) - kw.size();
    if (pos > i) return false;
    std::wstring sub = code.substr(pos, kw.size());
    bool left_ok = (pos == 0) || !iswalnum(code[pos - 1]);
    bool right_ok = (pos + kw.size() >= code.size()) || !iswalnum(code[pos + kw.size()]);
    return left_ok && right_ok && sub == kw;
}

// 簡易に class 名を収集する（"class <Name>" を探す）
std::unordered_set<std::wstring> CollectClassNames(const std::wstring& code)
{
    std::unordered_set<std::wstring> classes;
    size_t i = 0;
    const size_t n = code.size();
    while (i < n) {
        if (i + 5 <= n && code.substr(i, 5) == L"class") {
            bool left_ok = (i == 0) || !iswalnum(code[i - 1]);
            bool right_ok = (i + 5 >= n) || !iswalnum(code[i + 5]);
            if (left_ok && right_ok) {
                i += 5;
                while (i < n && iswspace(code[i])) ++i;
                std::wstring name;
                while (i < n && (iswalnum(code[i]) || code[i] == L'_')) {
                    name.push_back(code[i++]);
                }
                if (!name.empty()) classes.insert(name);
                continue;
            }
        }
        ++i;
    }
    return classes;
}

// タイプアウト（ハイライト付き）
// baseDelay: 基本遅延（ms）
// jitter: 文字ごとの最大ランダム追加/減少（ms）
void TypeOutHighlighted(const std::wstring& code, int baseDelay = 18, int jitter = 8, const ColorSet& cs = colors24)
{
    size_t i = 0;
    const size_t n = code.size();
    auto sleepMs = [&](int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); };

    // 乱数生成器（スレッドローカル）
    thread_local std::mt19937 rng((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(-jitter, jitter);

    auto classNames = CollectClassNames(code);

    while (i < n) {
        wchar_t c = code[i];

        // コメント (// ... 改行まで)
        if (c == L'/' && i + 1 < n && code[i + 1] == L'/') {
            std::wstring token;
            token.push_back(code[i++]);
            token.push_back(code[i++]);
            while (i < n && code[i] != L'\n') token.push_back(code[i++]);
            std::wcout << *cs.COMMENT;
            for (wchar_t ch : token) {
                std::wcout << ch << std::flush;
                sleepMs(std::max(1, baseDelay + dist(rng)));
            }
            std::wcout << *cs.RESET;
            continue;
        }

        // 文字列リテラル "..." または '...'
        if (c == L'"' || c == L'\'') {
            wchar_t quote = c;
            std::wstring token;
            token.push_back(code[i++]);
            while (i < n) {
                wchar_t ch = code[i++];
                token.push_back(ch);
                if (ch == quote) break;
                if (ch == L'\\' && i < n) { token.push_back(code[i++]); }
            }
            std::wcout << *cs.STRING;
            for (wchar_t ch : token) {
                std::wcout << ch << std::flush;
                sleepMs(std::max(1, baseDelay + dist(rng)));
            }
            std::wcout << *cs.RESET;
            continue;
        }

        // 識別子またはキーワード
        if (iswalpha(c) || c == L'_') {
            size_t start = i;
            std::wstring token;
            while (i < n && (iswalnum(code[i]) || code[i] == L'_')) token.push_back(code[i++]);

            // 次の非空白文字を覗いて '(' が来るかを判定（呼び出しの可能性）
            size_t j = i;
            while (j < n && iswspace(code[j])) ++j;

            bool isConstructorCall = (classNames.find(token) != classNames.end() && j < n && code[j] == L'(');
            bool isMethodName = false;
            if (start > 0 && code[start - 1] == L'.') isMethodName = true;
            bool isClassDeclarationName = PrevIsKeyword(code, start, L"class");

            const std::wstring* useColor = cs.NORMAL;
            if (keywords.find(token) != keywords.end() || isClassDeclarationName) {
                useColor = cs.KEYWORD;
            }
            else if (isClassDeclarationName) {
                useColor = cs.TYPE;
            }
            else if (isConstructorCall) {
                useColor = cs.CONS;
            }
            else if (isMethodName) {
                useColor = cs.METHOD;
            }
            else if (classNames.find(token) != classNames.end()) {
                useColor = cs.TYPE;
            }
            else {
                useColor = cs.NORMAL;
            }

            std::wcout << *useColor;
            for (wchar_t ch : token) {
                std::wcout << ch << std::flush;
                sleepMs(std::max(1, baseDelay + dist(rng)));
            }
            std::wcout << *cs.RESET;
            continue;
        }

        // 数字
        if (iswdigit(c)) {
            std::wstring token;
            while (i < n && (iswdigit(code[i]) || code[i] == L'.')) token.push_back(code[i++]);
            std::wcout << *cs.NUMBER;
            for (wchar_t ch : token) {
                std::wcout << ch << std::flush;
                sleepMs(std::max(1, baseDelay + dist(rng)));
            }
            std::wcout << *cs.RESET;
            continue;
        }

        // 空白・改行はそのまま出力（遅延は短め）
        if (c == L' ' || c == L'\t') {
            std::wcout << c << std::flush;
            i++;
            sleepMs(std::max(1, (baseDelay + dist(rng)) / 2));
            continue;
        }
        if (c == L'\n' || c == L'\r') {
            std::wcout << c << std::flush;
            i++;
            sleepMs(std::max(1, baseDelay + dist(rng)));
            continue;
        }

        // それ以外の記号（演算子や括弧など）
        std::wstring token;
        token.push_back(code[i++]);
        std::wcout << *cs.PUNCT;
        for (wchar_t ch : token) {
            std::wcout << ch << std::flush;
            sleepMs(std::max(1, baseDelay + dist(rng)));
        }
        std::wcout << *cs.RESET;
    }
}

void ShowEnter(const ColorSet& cs)
{
    std::wcout << *cs.NORMAL << L"[Enter]" << *cs.RESET << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(220));
}

int wmain(int argc, wchar_t** argv)
{
    std::setlocale(LC_ALL, "");
    bool use24 = TryEnableAnsiAndDecide24bit();
    const ColorSet& cs = use24 ? colors24 : colors8;

    if (argc != 2) {
        std::wcout << L"Usage: tsumugi-demo <scriptfile>" << std::endl;
        return 1;
    }

    std::wifstream file(argv[1]);
    if (!file) {
        std::wcout << *cs.COMMENT << L"Cannot open script file." << *cs.RESET << std::endl;
        return 1;
    }

    std::wstringstream buffer;
    buffer << file.rdbuf();
    std::wstring code = buffer.str();

    // タイプ速度のパラメータ（必要ならここを調整）
    int baseDelay = 50; // 基本遅延(ms)
    int jitter = 20;    // ±jitter(ms)

    TypeOutHighlighted(code, baseDelay, jitter, cs);
    std::wcout << std::endl;
    ShowEnter(cs);

    // --- 評価 ---
    auto env = std::make_shared<tsumugi::script::object::Environment>();
    auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(code.c_str());
    auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
    auto root = parser->ParseProgram();

    if (parser->GetLogger().HasAnyLog()) {
        std::wcout << *cs.COMMENT << L"Parse error." << *cs.RESET << std::endl;
        return 1;
    }

    auto evaluator = std::make_unique<tsumugi::script::evaluator::Evaluator>();
    auto result = evaluator->Eval(root.get(), env);

    if (result != nullptr) {
        // 結果はコンストラクタ色でプロンプト、文字列は文字列色で表示
        std::wcout << *cs.CONS << L"> " << *cs.RESET << *cs.STRING << result->Inspect() << *cs.RESET << std::endl;
    }

    return 0;
}

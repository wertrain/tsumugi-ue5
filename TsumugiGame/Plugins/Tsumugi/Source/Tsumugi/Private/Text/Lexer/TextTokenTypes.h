#pragma once

#include "Foundation/Types.h"

namespace tsumugi::text::lexer {

/// <summary>
/// トークン種類
/// </summary>
enum class TokenType : int32_t {

    /// <summary>
    /// 不正なトークン
    /// </summary>
    kIllegal,

    /// <summary>
    /// 終端ノード (End of File)
    /// </summary>
    kEOF,

    /// <summary>
    /// 記号以外の生の文字列・識別子
    /// （地の文、ラベル名、見出し名、タグ名、属性名、属性値のすべてがこれになります）
    /// </summary>
    kString,

    /// <summary>
    /// 改行
    /// </summary>
    kNewLine,

    /// <summary>
    /// 行頭のラベル定義マーク (*)
    /// </summary>
    kAsterisk,

    /// <summary>
    /// 行頭のコマンド定義マーク (@)
    /// </summary>
    kAtMark,

    /// <summary>
    /// ラベルの見出しセパレーター (|)
    /// </summary>
    kPipe,

    /// <summary>
    /// タグの開始カッコ ([)
    /// </summary>
    kTagOpen,

    /// <summary>
    /// タグの閉じカッコ (])
    /// </summary>
    kTagClose,

    /// <summary>
    /// タグ属性の代入文字 (=)
    /// </summary>
    kAssign,

    /// <summary>
    /// 行頭のコメントマーク (;)
    /// </summary>
    kSemiColon
};

/// <summary>
/// トークンタイプを文字列に変更する
/// </summary>
/// <param name="token_type"></param>
/// <returns></returns>
const tchar* TokenTypeToTString(const TokenType token_type);
const char* TokenTypeToString(const TokenType token_type);

}

#pragma once

#include "Foundation/Types.h"
#include "Script/Lexing/ScriptLexingTypes.h"

namespace tsumugi::script::lexing {

/// <summary>
/// トークン種類
/// </summary>
enum class TokenType : int32_t {

    /// --------- 特殊 --------- 

    /// <summary>
    /// 不正なトークン
    /// </summary>
    kIllegal,

    /// <summary>
    /// 終端
    /// </summary>
    kEOF,

    /// --------- 識別子 --------- 

    /// <summary>
    /// 識別子
    /// </summary>
    kIdentifier,

    /// <summary>
    /// 整数
    /// </summary>
    kInteger,

    /// <summary>
    /// 倍精度浮動小数点数
    /// </summary>
    kDouble,

    /// <summary>
    /// 文字列リテラル
    /// </summary>
    kString,

    /// --------- 演算子 --------- 

    /// <summary>
    /// = 演算子
    /// </summary>
    kAssign,

    /// <summary>
    /// + 演算子
    /// </summary>
    kPlus,

    /// <summary>
    /// - 演算子
    /// </summary>
    kMinus,

    /// <summary>
    /// * 演算子
    /// </summary>
    kAsterisk,

    /// <summary>
    /// / 演算子
    /// </summary>
    kSlash,

    /// <summary>
    /// ! 演算子
    /// </summary>
    kBang,

    /// <summary>
    /// < 演算子
    /// </summary>
    kLessThan,

    /// <summary>
    /// > 演算子
    /// </summary>
    kGreaterThan,

    /// <summary>
    /// <= 演算子
    /// </summary>
    kLessThanOrEqual,

    /// <summary>
    /// >= 演算子
    /// </summary>
    kGreaterThanOrEqual,

    /// <summary>
    /// == 演算子
    /// </summary>
    kEqual,

    /// <summary>
    /// != 演算子
    /// </summary>
    kNotEqual,

    /// <summary>
    /// | 論理和
    /// </summary>
    kLogicalDisjunction,

    /// <summary>
    /// & 論理積
    /// </summary>
    kLogicalConjunction,

    /// <summary>
    /// && 「かつ」結合
    /// </summary>
    kAnd,

    /// <summary>
    /// || 「または」結合
    /// </summary>
    kOr,

    /// --------- デリミタ --------- 
    /// 
    /// <summary>
    /// カンマデリミタ
    /// </summary>
    kComma,

    /// <summary>
    /// セミコロンデリミタ
    /// </summary>
    kSemicolon,

    /// --------- 括弧 --------- 

    /// <summary>
    /// 左丸括弧
    /// </summary>
    kLeftParenthesis,

    /// <summary>
    /// 右丸括弧
    /// </summary>
    kRightParenthesis,

    /// <summary>
    /// 左波括弧
    /// </summary>
    kLeftBraces,

    /// <summary>
    /// 右波括弧
    /// </summary>
    kRightBraces,

    /// <summary>
    /// 左角括弧
    /// </summary>
    kLeftBrackets,

    /// <summary>
    /// 右角括弧
    /// </summary>
    kRightBrackets,

    /// <summary>
    /// ダブルクォーテーション
    /// </summary>
    kStraightQuotes,

    /// --------- キーワード --------- 

    /// <summary>
    /// 関数キーワード
    /// </summary>
    kFunction,

    /// <summary>
    /// 変数宣言キーワード
    /// </summary>
    kLet,

    /// <summary>
    /// if キーワード
    /// </summary>
    kIf,

    /// <summary>
    /// else キーワード
    /// </summary>
    kElse,

    /// <summary>
    /// return キーワード
    /// </summary>
    kReturn,

    /// <summary>
    /// true キーワード
    /// </summary>
    kTrue,

    /// <summary>
    /// false キーワード
    /// </summary>
    kFalse
};

/// <summary>
/// 識別子かどうか判定する
/// </summary>
/// <param name="identifier"></param>
/// <returns></returns>
const TokenType LookupIdentifier(const tstring& identifier);

/// <summary>
/// トークンタイプを文字列に変更する
/// </summary>
/// <param name="token_type"></param>
/// <returns></returns>
const tchar* TokenTypeToTString(const TokenType token_type);
const char* TokenTypeToString(const TokenType token_type);

}

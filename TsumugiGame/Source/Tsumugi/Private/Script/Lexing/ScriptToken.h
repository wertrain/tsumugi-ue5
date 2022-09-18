#pragma once

#include "Foundation/Types.h"

namespace tsumugi::script::lexing {

    /// <summary>
    /// トークン種類
    /// </summary>
    enum class TokenType {

        /// <summary>
        /// 不正なトークン
        /// </summary>
        kIllegal,

        /// <summary>
        /// 
        /// </summary>
        kEOF,

        /// <summary>
        /// テキスト
        /// </summary>
        kText,

        /// <summary>
        /// タグ
        /// </summary>
        kTag,

        /// <summary>
        /// タグ終了
        /// </summary>
        kTagEnd,

        /// <summary>
        /// タグの属性名
        /// </summary>
        kTagAttributeName,

        /// <summary>
        /// タグの属性値
        /// </summary>
        kTagAttributeValue,

        /// <summary>
        /// ラベル
        /// </summary>
        kLabel,

        /// <summary>
        /// ラベルの見出し
        /// </summary>
        kLabelHeadline,

        /// <summary>
        /// 代入文字
        /// </summary>
        kAssign
    };

    /// <summary>
    /// トークン
    /// </summary>
    class Token {
    public:
        Token();
        Token(const TokenType token_type, const tstring& literal_string);

        const TokenType GetTokenType() const;
        void SetTokenType(const TokenType token_type);
        
        const tstring& GetLiteral() const;
        void SetLiteral(const tstring& literal_string);

    private:
        TokenType token_type_;
        tstring literal_;
    };
}

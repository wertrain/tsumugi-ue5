#pragma once

#include "Foundation/Types.h"

namespace tsumugi::script::lexing {

    /// <summary>
    /// �g�[�N�����
    /// </summary>
    enum class TokenType {

        /// <summary>
        /// �s���ȃg�[�N��
        /// </summary>
        kIllegal,

        /// <summary>
        /// 
        /// </summary>
        kEOF,

        /// <summary>
        /// �e�L�X�g
        /// </summary>
        kText,

        /// <summary>
        /// �^�O
        /// </summary>
        kTag,

        /// <summary>
        /// �^�O�I��
        /// </summary>
        kTagEnd,

        /// <summary>
        /// �^�O�̑�����
        /// </summary>
        kTagAttributeName,

        /// <summary>
        /// �^�O�̑����l
        /// </summary>
        kTagAttributeValue,

        /// <summary>
        /// ���x��
        /// </summary>
        kLabel,

        /// <summary>
        /// ���x���̌��o��
        /// </summary>
        kLabelHeadline,

        /// <summary>
        /// �������
        /// </summary>
        kAssign
    };

    /// <summary>
    /// �g�[�N��
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

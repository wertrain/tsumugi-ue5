#pragma once

#include "Foundation/Types.h"
#include "Script/Lexing/ScriptLexingTypes.h"

namespace tsumugi::script::lexing {

/// <summary>
/// �g�[�N�����
/// </summary>
enum class TokenType : int32_t {

    /// --------- ���� --------- 

    /// <summary>
    /// �s���ȃg�[�N��
    /// </summary>
    kIllegal,

    /// <summary>
    /// �I�[
    /// </summary>
    kEOF,

    /// --------- ���ʎq --------- 

    /// <summary>
    /// ���ʎq
    /// </summary>
    kIdentifier,

    /// <summary>
    /// ����
    /// </summary>
    kInteger,

    /// <summary>
    /// �{���x���������_��
    /// </summary>
    kDouble,

    /// <summary>
    /// �����񃊃e����
    /// </summary>
    kString,

    /// --------- ���Z�q --------- 

    /// <summary>
    /// = ���Z�q
    /// </summary>
    kAssign,

    /// <summary>
    /// + ���Z�q
    /// </summary>
    kPlus,

    /// <summary>
    /// - ���Z�q
    /// </summary>
    kMinus,

    /// <summary>
    /// * ���Z�q
    /// </summary>
    kAsterisk,

    /// <summary>
    /// / ���Z�q
    /// </summary>
    kSlash,

    /// <summary>
    /// ! ���Z�q
    /// </summary>
    kBang,

    /// <summary>
    /// < ���Z�q
    /// </summary>
    kLessThan,

    /// <summary>
    /// > ���Z�q
    /// </summary>
    kGreaterThan,

    /// <summary>
    /// <= ���Z�q
    /// </summary>
    kLessThanOrEqual,

    /// <summary>
    /// >= ���Z�q
    /// </summary>
    kGreaterThanOrEqual,

    /// <summary>
    /// == ���Z�q
    /// </summary>
    kEqual,

    /// <summary>
    /// != ���Z�q
    /// </summary>
    kNotEqual,

    /// <summary>
    /// | �_���a
    /// </summary>
    kLogicalDisjunction,

    /// <summary>
    /// & �_����
    /// </summary>
    kLogicalConjunction,

    /// <summary>
    /// && �u���v����
    /// </summary>
    kAnd,

    /// <summary>
    /// || �u�܂��́v����
    /// </summary>
    kOr,

    /// --------- �f���~�^ --------- 
    /// 
    /// <summary>
    /// �J���}�f���~�^
    /// </summary>
    kComma,

    /// <summary>
    /// �Z�~�R�����f���~�^
    /// </summary>
    kSemicolon,

    /// --------- ���� --------- 

    /// <summary>
    /// ���ۊ���
    /// </summary>
    kLeftParenthesis,

    /// <summary>
    /// �E�ۊ���
    /// </summary>
    kRightParenthesis,

    /// <summary>
    /// ���g����
    /// </summary>
    kLeftBraces,

    /// <summary>
    /// �E�g����
    /// </summary>
    kRightBraces,

    /// <summary>
    /// ���p����
    /// </summary>
    kLeftBrackets,

    /// <summary>
    /// �E�p����
    /// </summary>
    kRightBrackets,

    /// <summary>
    /// �_�u���N�H�[�e�[�V����
    /// </summary>
    kStraightQuotes,

    /// --------- �L�[���[�h --------- 

    /// <summary>
    /// �֐��L�[���[�h
    /// </summary>
    kFunction,

    /// <summary>
    /// �ϐ��錾�L�[���[�h
    /// </summary>
    kLet,

    /// <summary>
    /// if �L�[���[�h
    /// </summary>
    kIf,

    /// <summary>
    /// else �L�[���[�h
    /// </summary>
    kElse,

    /// <summary>
    /// return �L�[���[�h
    /// </summary>
    kReturn,

    /// <summary>
    /// true �L�[���[�h
    /// </summary>
    kTrue,

    /// <summary>
    /// false �L�[���[�h
    /// </summary>
    kFalse
};

/// <summary>
/// ���ʎq���ǂ������肷��
/// </summary>
/// <param name="identifier"></param>
/// <returns></returns>
const TokenType LookupIdentifier(const tstring& identifier);

/// <summary>
/// �g�[�N���^�C�v�𕶎���ɕύX����
/// </summary>
/// <param name="token_type"></param>
/// <returns></returns>
const tchar* TokenTypeToTString(const TokenType token_type);
const char* TokenTypeToString(const TokenType token_type);

}

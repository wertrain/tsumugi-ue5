#pragma once

#include "Foundation/Types.h"

namespace tsumugi::script::ast {

class INode {
public:
    INode();
    virtual ~INode();

    /// <summary>
    /// �g�[�N���̃��e����
    /// </summary>
    /// <returns>�g�[�N���̃��e����</returns>
    virtual tstring TokenLiteral() const = 0;

    /// <summary>
    /// �R�[�h�ɕϊ�
    /// </summary>
    /// <returns>�R�[�h</returns>
    virtual tstring ToCode() const = 0;
};

}

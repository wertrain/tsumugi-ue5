#pragma once

#include "Foundation/Types.h"

namespace tsumugi::script::ast {

class INode {
public:
    INode();
    virtual ~INode();

    /// <summary>
    /// トークンのリテラル
    /// </summary>
    /// <returns>トークンのリテラル</returns>
    virtual tstring TokenLiteral() const = 0;

    /// <summary>
    /// コードに変換
    /// </summary>
    /// <returns>コード</returns>
    virtual tstring ToCode() const = 0;
};

}

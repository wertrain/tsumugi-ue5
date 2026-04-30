#pragma once

#include "Foundation/Types.h"

namespace tsumugi::script::ast {

enum class NodeType {
    kProgram,
    kIntegerLiteral,
    kStringLiteral,
    kBooleanLiteral,
    kIdentifier,
    kPrefixExpression,
    kInfixExpression,
    kIfExpression,
    kLetStatement,
    kReturnStatement,
    kCallExpression,
    kFunctionLiteral,
    kExpressionStatement,
    kBlockStatement,
    Num
};

class INode {
public:
    INode();
    virtual ~INode();

    /// <summary>
    /// ノードタイプを取得
    /// </summary>
    /// <returns>ノードタイプ</returns>
    virtual NodeType GetNodeType() const = 0;

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

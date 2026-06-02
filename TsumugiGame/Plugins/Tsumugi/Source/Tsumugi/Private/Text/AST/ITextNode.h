#pragma once

#include "Foundation/Types.h"

namespace tsumugi::text::ast {

enum class NodeType {
    kProgram,
    kLabelStatement,
    Num
};

class INode {
public:
    INode();
    virtual ~INode();

    /// <summary>
    /// 繝弱・繝峨ち繧､繝励ｒ蜿門ｾ・
    /// </summary>
    /// <returns>繝弱・繝峨ち繧､繝・/returns>
    virtual NodeType GetNodeType() const = 0;

    /// <summary>
    /// 繝医・繧ｯ繝ｳ縺ｮ繝ｪ繝・Λ繝ｫ
    /// </summary>
    /// <returns>繝医・繧ｯ繝ｳ縺ｮ繝ｪ繝・Λ繝ｫ</returns>
    virtual tstring TokenLiteral() const = 0;

    /// <summary>
    /// 繧ｳ繝ｼ繝峨↓螟画鋤
    /// </summary>
    /// <returns>繧ｳ繝ｼ繝・/returns>
    virtual tstring ToCode() const = 0;
};

}

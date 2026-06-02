#pragma once

#include "Foundation/Types.h"
#include "Text/AST/ITextNode.h"
#include "Text/AST/ITextStatement.h"

namespace tsumugi::text::ast {

/// <summary>
/// 木構造 Root
/// </summary>
class Program : public INode {
public:
    Program();
    ~Program();
    NodeType GetNodeType() const final override { return NodeType::kProgram; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;
    const IStatement* GetStatement(int index) const;
    const tarray<std::unique_ptr<IStatement>>& GetStatements() const;
    void AddStatement(std::unique_ptr<IStatement> statement);
    size_t GetStatementCount() const;
private:
    tarray<std::unique_ptr<IStatement>> statements_;
};

}

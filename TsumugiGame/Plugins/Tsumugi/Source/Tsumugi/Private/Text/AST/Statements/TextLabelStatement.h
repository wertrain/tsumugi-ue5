#pragma once

#include "Foundation/Types.h"
#include "Text/AST/ITextStatement.h"

namespace tsumugi::text::lexer { class Token; }

namespace tsumugi::text::ast::statement {

/// <summary>
/// Label 文
/// *（ラベル名）|（ラベル見出し）
/// </summary>
class LabelStatement : public IStatement {
public:
    LabelStatement(std::shared_ptr<lexer::Token> token);
    ~LabelStatement() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }
    const tstring& GetLabelName() const { return labelName_; }
    void SetLabelName(const tstring &name) { labelName_ = name; }
    const tstring& GetLabelHeadline() const { return labelHeadline_; }
    void SetLabelHeadline(const tstring &headline) { labelHeadline_ = headline; }

    NodeType GetNodeType() const final override { return NodeType::kLabelStatement; }
    tstring TokenLiteral() const override final;
    tstring ToCode() const override final;

private:
    std::shared_ptr<lexer::Token> token_;
    tstring labelName_;
    tstring labelHeadline_;
};

}

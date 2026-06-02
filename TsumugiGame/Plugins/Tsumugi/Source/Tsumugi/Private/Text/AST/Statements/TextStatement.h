#pragma once

#include "Foundation/Types.h"
#include "Text/AST/ITextStatement.h"

namespace tsumugi::text::lexer { class Token; }

namespace tsumugi::text::ast::statement {

/// <summary>
/// Text 文
/// *
/// </summary>
class TextStatement : public IStatement {
public:
    TextStatement(std::shared_ptr<lexer::Token> token);
    ~TextStatement() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }
    const tstring& GetText() const { return text_; }
    void SetText(const tstring &text) { text_ = text; }

    NodeType GetNodeType() const final override { return NodeType::kTextStatement; }
    tstring TokenLiteral() const override final;
    tstring ToCode() const override final;

private:
    std::shared_ptr<lexer::Token> token_;
    tstring text_;
};

}

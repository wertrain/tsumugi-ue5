#pragma once

#include "Foundation/Types.h"
#include "Text/AST/ITextStatement.h"
#include <unordered_map>

namespace tsumugi::text::lexer { class Token; }

namespace tsumugi::text::ast::statement {

/// <summary>
/// Tag 文
/// [タグ名( 属性名=値)...
/// </summary>
class TagStatement : public IStatement {
public:
    TagStatement(std::shared_ptr<lexer::Token> token);
    ~TagStatement() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }
    const tstring& GetTagName() const { return tagName_; }
    void SetTagName(const tstring &name) { tagName_ = name; }
    const std::unordered_map<tstring, tstring>& GetAttributes() const { return attributes_; }
    void AddAttribute(const tstring& key, const tstring& value) { attributes_[key] = value; }

    NodeType GetNodeType() const final override { return NodeType::kTagStatement; }
    tstring TokenLiteral() const override final;
    tstring ToCode() const override final;

private:
    std::shared_ptr<lexer::Token> token_;
    tstring tagName_;
    std::unordered_map<tstring, tstring> attributes_;
};

}

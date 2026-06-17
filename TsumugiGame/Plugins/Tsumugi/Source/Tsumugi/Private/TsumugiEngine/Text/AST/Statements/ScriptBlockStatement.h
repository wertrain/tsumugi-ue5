#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Text/AST/ITextStatement.h"
#include <unordered_map>

namespace tsumugi::text::lexer { class Token; }

namespace tsumugi::text::ast::statement {

/// <summary>
/// インラインスクリプト 文
/// [iscript]...[endscript]
/// </summary>
class ScriptBlockStatement : public IStatement {
public:
    ScriptBlockStatement(std::shared_ptr<lexer::Token> token);
    ~ScriptBlockStatement() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }
    const tstring& GetTagName() const { return tagName_; }
    void SetTagName(const tstring &name) { tagName_ = name; }
    const tstring& GetScriptText() const { return scriptText_; }
    void SetScriptText(const tstring &scriptText) { scriptText_ = scriptText; }

    NodeType GetNodeType() const final override { return NodeType::kScriptBlockStatement; }
    tstring TokenLiteral() const override final;
    tstring ToCode() const override final;

private:
    std::shared_ptr<lexer::Token> token_;
    tstring tagName_;
    tstring scriptText_;
};

}

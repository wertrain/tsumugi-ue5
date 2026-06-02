#pragma once

#include "Foundation/Types.h"
#include "Script/AST/IExpression.h"

namespace tsumugi::script::lexer { class Token; }
namespace tsumugi::script::ast::expression { class Identifier; }

namespace tsumugi::script::ast::expression {

// PropertyAccessExpression
// -----------------------------------------------------------------------------
// tsumugi 縺ｫ縺翫￠繧九後ラ繝・ヨ縺ｫ繧医ｋ繝励Ο繝代ユ繧｣繧｢繧ｯ繧ｻ繧ｹ縲阪ｒ陦ｨ縺・AST 繝弱・繝峨・
// 
//     obj.x
// 
// 縺ｮ繧医≧縺ｪ讒区枚繧呈桶縺・・onkey 縺ｫ縺ｯ蟄伜惠縺励↑縺・》sumugi 迢ｬ閾ｪ縺ｮ讒区枚隕∫ｴ縲・
//
// 縲先ｧ区枚荳翫・菴咲ｽｮ縺･縺代・
// - obj["x"] 繧定｡ｨ縺・IndexExpression 縺ｨ縺ｯ蛻･繝弱・繝峨→縺励※螳夂ｾｩ縺輔ｌ繧九・
// - 縺､縺ｾ繧顔ｳ冶｡｣讒区枚縺ｧ縺ｯ縺ｪ縺上∵ｧ区枚繝ｬ繝吶Ν縺ｧ譏守｢ｺ縺ｫ蛹ｺ蛻･縺輔ｌ繧九・
//
// 縲先э蜻ｳ隲紋ｸ翫・謇ｱ縺・・
// - 繝励Ο繝代ユ繧｣縺ｮ蜿門ｾ励◎縺ｮ繧ゅ・縺ｯ ObjectProtocolDispatcher 縺ｫ蟋碑ｭｲ縺輔ｌ縲・
//   Dispatcher 縺ｯ縲悟､繧定ｿ斐☆縺縺代阪ｒ諡・ｽ薙☆繧九・
// - 繝｡繧ｽ繝・ラ蛹厄ｼ・his 縺ｮ豕ｨ蜈･・峨・ PropertyAccess 縺ｧ縺ｯ陦後ｏ縺ｪ縺・・
// - obj.foo() / super.foo() 縺ｮ繧医≧縺ｪ縲悟他縺ｳ蜃ｺ縺玲凾縺ｮ繝ｬ繧ｷ繝ｼ繝先ｱｺ螳壹阪・
//   EvalCallExpression 蛛ｴ縺ｧ陦後ｏ繧後！nvoke 縺・receiver 繧貞女縺大叙縺｣縺ｦ螳溯｡後☆繧九・
// 
// 縲先僑蠑ｵ諤ｧ縲・
// - UserObject・医け繝ｩ繧ｹ/繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ・峨→繧り・辟ｶ縺ｫ邨ｱ蜷医＆繧後・
//   obj.method() / super.method() 縺ｮ繧医≧縺ｪ繝｡繧ｽ繝・ラ蜻ｼ縺ｳ蜃ｺ縺励Δ繝・Ν縺ｮ蝓ｺ逶､縺ｨ縺ｪ繧九・
// - 繝励Ο繝代ユ繧｣繧｢繧ｯ繧ｻ繧ｹ縺ｯ縲悟､繧貞叙繧九□縺代阪↓蠕ｹ縺吶ｋ縺溘ａ縲・
//   蜻ｼ縺ｳ蜃ｺ縺励Δ繝・Ν縺ｮ荳雋ｫ諤ｧ縺ｨ諡｡蠑ｵ諤ｧ縺碁ｫ倥＞縲・
// 
// tsumugi 縺ｮ繧ｪ繝悶ず繧ｧ繧ｯ繝医Δ繝・Ν縺ｫ縺翫￠繧九∵怙繧ょ渕譛ｬ逧・↑讒区枚繝弱・繝峨・縺ｲ縺ｨ縺､縲・
// -----------------------------------------------------------------------------
class PropertyAccessExpression : public IExpression {
public:
    PropertyAccessExpression(std::shared_ptr<lexer::Token> token, std::unique_ptr<ast::IExpression> left);
    PropertyAccessExpression(std::shared_ptr<lexer::Token> token, std::unique_ptr<ast::IExpression> left, std::unique_ptr<ast::expression::Identifier> name);
    ~PropertyAccessExpression() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }
    const ast::IExpression* GetLeft() const { return left_.get(); }
    void SetLeft(std::unique_ptr<ast::IExpression> value) { left_ = std::move(value); }
    const ast::expression::Identifier* GetName() const { return name_.get(); }
    void SetName(std::unique_ptr<ast::expression::Identifier> value) { name_ = std::move(value); }

    NodeType GetNodeType() const final override { return NodeType::kPropertyAccessExpression; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<lexer::Token> token_;
    std::unique_ptr<ast::IExpression> left_;
    std::unique_ptr<ast::expression::Identifier> name_;
};

}

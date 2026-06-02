#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <functional>

namespace tsumugi::script::ast::expression { class Identifier; }
namespace tsumugi::script::ast::statement { class BlockStatement; }
namespace tsumugi::script::object { class Environment; }
namespace tsumugi::script::object { class ClassObject; }

namespace tsumugi::script::object {

// UserFunctionObject 縺ｯ Monkey 縺ｮ FunctionObject 縺ｫ逶ｸ蠖薙＠縲・
// tsumugi 縺ｫ縺翫￠繧九檎ｴ皮ｲ九↑髢｢謨ｰ縲阪ｒ陦ｨ縺吶・
// 
// 縲占ｨｭ險井ｸ翫・諢丞峙縲・
// - UserFunctionObject 縺ｯ縲悟ｮ夂ｾｩ譎ゅ・迺ｰ蠅・ｼ・nvironment・峨阪□縺代ｒ菫晄戟縺励・
//   this/self 縺ｮ讎ょｿｵ繧貞・驛ｨ縺ｫ縺ｯ謖√◆縺ｪ縺・・
// - 繝｡繧ｽ繝・ラ蜻ｼ縺ｳ蜃ｺ縺玲凾縺ｮ this 縺ｮ豕ｨ蜈･縺ｯ Evaluator 蛛ｴ縺ｧ陦後ｏ繧後・
//   UserFunctionObject 閾ｪ菴薙・ 窶憺未謨ｰ縺ｨ縺励※縺ｮ譛ｬ菴凪・縺ｮ縺ｿ繧呈球蠖薙☆繧九・
// 
// 縲舌Γ繧ｽ繝・ラ縺ｨ縺励※菴ｿ繧上ｌ繧句ｴ蜷医・
// - ClassObject 縺ｫ逋ｻ骭ｲ縺輔ｌ縺・UserFunctionObject 縺ｯ縲後Γ繧ｽ繝・ラ譛ｬ菴薙阪→縺励※謇ｱ繧上ｌ繧九・
// - 蜻ｼ縺ｳ蜃ｺ縺玲凾縺ｫ縺ｯ Evaluator 縺・receiver・・his・峨ｒ譁ｰ縺励＞ Environment 縺ｫ譚溽ｸ帙＠縲・
//   縺昴・迺ｰ蠅・〒 UserFunctionObject 縺ｮ body 繧貞ｮ溯｡後☆繧九・
// - 縺､縺ｾ繧翫ゞserFunctionObject 縺ｯ縲後Γ繧ｽ繝・ラ蛹悶・莉慕ｵ・∩縲阪ｒ謖√◆縺壹・
//   Evaluator 縺碁未謨ｰ繧・窶懊Γ繧ｽ繝・ラ縺ｨ縺励※螳溯｡後☆繧銀・蠖ｹ蜑ｲ繧呈球縺・・
// 
// 縲尽uper 蟇ｾ蠢懊・縺溘ａ縺ｮ諡｡蠑ｵ縲・
// - 繝｡繧ｽ繝・ラ縺後←縺ｮ繧ｯ繝ｩ繧ｹ縺ｫ螻槭＠縺ｦ縺・ｋ縺九ｒ遏･繧句ｿ・ｦ√′縺ゅｋ縺溘ａ縲・
//   UserFunctionObject 縺ｯ ownerClass_ 繧剃ｿ晄戟縺吶ｋ縲・
// - super.foo() 縺ｮ隗｣豎ｺ縺ｯ Evaluator 蛛ｴ縺ｧ陦後ｏ繧後・
//   縲檎樟蝨ｨ螳溯｡御ｸｭ縺ｮ繝｡繧ｽ繝・ラ縺ｮ ownerClass 竊・隕ｪ繧ｯ繝ｩ繧ｹ 竊・prototype縲・
//   縺ｨ縺・≧髫主ｱ､繧定ｾｿ縺｣縺ｦ繝｡繧ｽ繝・ラ繧呈､懃ｴ｢縺吶ｋ縲・
// - UserFunctionObject 閾ｪ菴薙・ super 縺ｮ隗｣豎ｺ繝ｭ繧ｸ繝・け繧呈戟縺溘★縲・
//   縺ゅ￥縺ｾ縺ｧ 窶懷ｮ夂ｾｩ譎ゅせ繧ｳ繝ｼ繝励ｒ謖√▽邏皮ｲ九↑髢｢謨ｰ窶・縺ｨ縺励※謖ｯ繧玖・縺・・
// 
// 縲舌Γ繝ｪ繝・ヨ縲・
// - 髢｢謨ｰ繧ｪ繝悶ず繧ｧ繧ｯ繝医ｒ豎壹＆縺壹・未謨ｰ縺ｨ繝｡繧ｽ繝・ラ縺ｮ雋ｬ蜍吶ｒ譏守｢ｺ縺ｫ蛻・屬縺ｧ縺阪ｋ縲・
// - this / super / 邯呎価縺ｨ縺・▲縺・OOP 縺ｮ莉慕ｵ・∩繧・Evaluator 縺ｨ ClassObject 縺ｫ髢峨§霎ｼ繧√ｉ繧後ｋ縲・
// - Monkey 縺ｮ縲碁未謨ｰ縺ｯ縺溘□縺ｮ蛟､縲阪→縺・≧蜩ｲ蟄ｦ繧剃ｿ昴■縺､縺､縲・
//   繧ｯ繝ｩ繧ｹ繝吶・繧ｹ OOP 繧定・辟ｶ縺ｫ諡｡蠑ｵ縺ｧ縺阪ｋ縲・
// 
// 縺薙ｌ縺ｫ繧医ｊ UserFunctionObject 縺ｯ縲・
// 縲悟ｮ夂ｾｩ譎ゅせ繧ｳ繝ｼ繝励ｒ謖√▽邏皮ｲ九↑髢｢謨ｰ縲・
// 縺九▽
// 縲後け繝ｩ繧ｹ髫主ｱ､縺ｫ螻槭☆繧九Γ繧ｽ繝・ラ縺ｮ譛ｬ菴薙・
// 縺ｮ荳｡譁ｹ繧貞ｮ牙・縺ｫ諡・≧縲》sumugi 縺ｮ髢｢謨ｰ繝ｻ繝｡繧ｽ繝・ラ繝｢繝・Ν縺ｮ蝓ｺ逶､縺ｨ縺ｪ繧九・

class UserFunctionObject : public IObject {
public:
    // parameters + body + environment 縺ｮ邨・∩蜷医ｏ縺帙・ Monkey 縺ｨ蜷後§縲・
    // environment 縺ｯ縲悟ｮ夂ｾｩ譎ゅ・繧ｹ繧ｳ繝ｼ繝励阪ｒ菫晄戟縺吶ｋ縺溘ａ縺ｮ繧ｯ繝ｭ繝ｼ繧ｸ繝｣迺ｰ蠅・・
    explicit UserFunctionObject();
    explicit UserFunctionObject(std::vector<std::shared_ptr<ast::expression::Identifier>> parameters, std::shared_ptr<ast::statement::BlockStatement> body, std::shared_ptr<object::Environment> environment);

    const std::vector<std::shared_ptr<ast::expression::Identifier>>& GetParameters() const { return parameters_; }
    void SetParameters(const std::vector<std::shared_ptr<ast::expression::Identifier>>& parameters) { parameters_ = parameters; }
    void AddParameter(std::shared_ptr<ast::expression::Identifier> parameter) { parameters_.push_back(std::move(parameter)); }
    const std::shared_ptr<const ast::statement::BlockStatement>& GetBody() const { return body_; }
    void SetBody(std::shared_ptr<const ast::statement::BlockStatement> body) { body_ = std::move(body); }
    std::shared_ptr<object::Environment> GetEnvironment() const { return environment_; }
    void SetEnvironment(std::shared_ptr<object::Environment> environment) { environment_ = std::move(environment); }
    std::shared_ptr<object::ClassObject> GetOwnerClass() const { return ownerClass_; }
    void SetOwnerClass(std::shared_ptr<object::ClassObject> c) { ownerClass_ = c; }

    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    std::vector<std::shared_ptr<ast::expression::Identifier>> parameters_;
    std::shared_ptr<const ast::statement::BlockStatement> body_;
    std::shared_ptr<object::Environment> environment_;
    std::shared_ptr<object::ClassObject> ownerClass_;
};

}

#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <functional>

namespace tsumugi::script::object {

using BuiltinFunctionType = std::function<std::shared_ptr<object::IObject>(std::shared_ptr<object::IObject>, const std::vector<std::shared_ptr<object::IObject>>&)>;

// BuiltinFunctionObject 縺ｯ tsumugi 縺ｫ縺翫￠繧九檎ｵ・∩霎ｼ縺ｿ髢｢謨ｰ縲阪ｒ陦ｨ縺吶・
// 
// 縲仙ｽｹ蜑ｲ縲・
// - C++ 蛛ｴ縺ｧ螳夂ｾｩ縺輔ｌ縺滄未謨ｰ・・eys, values, len, print 縺ｪ縺ｩ・峨ｒ
//   繧ｹ繧ｯ繝ｪ繝励ヨ縺九ｉ蜻ｼ縺ｳ蜃ｺ縺帙ｋ繧医≧縺ｫ縺吶ｋ縺溘ａ縺ｮ繧ｪ繝悶ず繧ｧ繧ｯ繝医・
// - 繧ｰ繝ｭ繝ｼ繝舌Ν遨ｺ髢薙↓逋ｻ骭ｲ縺輔ｌ縲・nvironment 縺九ｉ蜿ら・縺輔ｌ繧九・
// - UserFunctionObject 縺ｨ逡ｰ縺ｪ繧翫∝ｮ夂ｾｩ譎ゅせ繧ｳ繝ｼ繝暦ｼ・nvironment・峨ｒ謖√◆縺壹・
//   縺溘□ 1 縺､縺ｮ C++ 髢｢謨ｰ繝昴う繝ｳ繧ｿ・・unction_・峨□縺代ｒ菫晄戟縺吶ｋ縲・
// 
// 縲仙他縺ｳ蜃ｺ縺励Δ繝・Ν縲・
// - Evaluator 縺ｯ BuiltinFunctionObject 繧定ｦ九▽縺代ｋ縺ｨ縲・
//     function_(receiver, args)
//   繧堤峩謗･蜻ｼ縺ｳ蜃ｺ縺吶・
// - receiver 縺ｯ繝｡繧ｽ繝・ラ蜻ｼ縺ｳ蜃ｺ縺玲凾縺ｮ this・・bj.method() 縺ｮ obj・峨・
//   繧ｰ繝ｭ繝ｼ繝舌Ν髢｢謨ｰ縺ｨ縺励※蜻ｼ縺ｶ蝣ｴ蜷医・ null 縺梧ｸ｡縺輔ｌ繧九・
// 
// 縲占ｨｭ險井ｸ翫・諢丞峙縲・
// - 邨・∩霎ｼ縺ｿ髢｢謨ｰ縺ｯ縲瑚ｨ隱槭Ξ繝吶Ν縺ｮ髢｢謨ｰ縲阪〒縺ｯ縺ｪ縺上・
//   縲後・繧ｹ繝郁ｨ隱橸ｼ・++・牙・縺ｮ讖溯・繧偵せ繧ｯ繝ｪ繝励ヨ縺ｫ蜈ｬ髢九☆繧九◆繧√・讖区ｸ｡縺励阪・
// - UserFunctionObject・医せ繧ｯ繝ｪ繝励ヨ縺ｧ螳夂ｾｩ縺輔ｌ縺滄未謨ｰ・峨→縺ｯ譏守｢ｺ縺ｫ蛻・屬縺励・
//   繧ｯ繝ｭ繝ｼ繧ｸ繝｣迺ｰ蠅・ｄ ownerClass 縺ｪ縺ｩ縺ｮ讎ょｿｵ繧呈戟縺溘↑縺・・
// 
// 縲舌Γ繝ｪ繝・ヨ縲・
// - 邨・∩霎ｼ縺ｿ髢｢謨ｰ縺ｮ霑ｽ蜉縺悟ｮｹ譏難ｼ・++ 縺ｮ繝ｩ繝繝繧堤匳骭ｲ縺吶ｋ縺縺托ｼ峨・
// - UserFunctionObject 縺ｨ雋ｬ蜍吶′蛻・屬縺輔ｌ縲・未謨ｰ繝｢繝・Ν縺後す繝ｳ繝励Ν縺ｫ縺ｪ繧九・
// - receiver 繧貞女縺大叙繧後ｋ縺溘ａ縲√Γ繧ｽ繝・ラ逧・↑邨・∩霎ｼ縺ｿ髢｢謨ｰ繧ょｮ溯｣・庄閭ｽ縲・
// 
// 縺薙ｌ縺ｫ繧医ｊ BuiltinFunctionObject 縺ｯ縲・
// 縲靴++ 蛛ｴ縺ｮ讖溯・繧偵せ繧ｯ繝ｪ繝励ヨ縺ｫ蜈ｬ髢九☆繧九◆繧√・譛蟆城剞縺ｮ髢｢謨ｰ繧ｪ繝悶ず繧ｧ繧ｯ繝医・
// 縺ｨ縺励※讖溯・縺励》sumugi 縺ｮ邨・∩霎ｼ縺ｿ API 縺ｮ蝓ｺ逶､縺ｨ縺ｪ繧九・

class BuiltinFunctionObject : public IObject {
public:
    explicit BuiltinFunctionObject(BuiltinFunctionType fn);
    BuiltinFunctionType GetFunction() const { return function_; }
    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    BuiltinFunctionType function_;
};

}

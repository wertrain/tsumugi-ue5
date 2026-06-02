#include "Script/Objects/Environment.h"
#include "Script/Objects/IObject.h"


// 縲仙ｮ夂ｾｩ譎ら腸蠅・ｼ・efinition environment・峨・
// 髢｢謨ｰ縺悟ｮ夂ｾｩ縺輔ｌ縺溘→縺阪・ Environment縲・
// UserFunctionObject 縺御ｿ晄戟縺吶ｋ縲・
// 繧ｯ繝ｭ繝ｼ繧ｸ繝｣繧貞ｮ溽樟縺吶ｋ縺溘ａ縺ｫ蠢・ｦ√・
//
// 縲仙ｮ溯｡梧凾迺ｰ蠅・ｼ・xecution environment・峨・
// InvokeFunction 縺碁未謨ｰ繧貞他縺ｳ蜃ｺ縺吶→縺阪↓菴懊ｋ譁ｰ縺励＞ Environment縲・
// - 蠑墓焚譚溽ｸ・
// - self 縺ｮ豕ｨ蜈･・医Γ繧ｽ繝・ラ蜻ｼ縺ｳ蜃ｺ縺玲凾・・
// 縺ｪ縺ｩ繧定｡後≧縲・

namespace tsumugi::script::object {

// 繝｡繧ｽ繝・ラ縺ｮ繝ｬ繧ｷ繝ｼ繝舌・蜷阪→縺励※繧ｻ繝・ヨ縺吶ｋ譁・ｭ怜・
const tstring Environment::kThis = TT("this");

// Environment()
// -----------------------------------------------------------------------------
// 繧ｰ繝ｭ繝ｼ繝舌Ν繧ｹ繧ｳ繝ｼ繝励ｄ譛蛻昴・繝ｭ繝ｼ繧ｫ繝ｫ繧ｹ繧ｳ繝ｼ繝励ｒ菴懊ｋ縺ｨ縺阪↓菴ｿ縺・・
// outer_ 繧呈戟縺溘↑縺・後Ν繝ｼ繝育腸蠅・阪・
// -----------------------------------------------------------------------------
Environment::Environment()
    : store_()
    , outer_()
    , currentFunction_() {

}

// Environment(outer)
// -----------------------------------------------------------------------------
// 譁ｰ縺励＞繧ｹ繧ｳ繝ｼ繝励ｒ菴懊ｋ縺ｨ縺阪↓菴ｿ縺・・
// 萓具ｼ・
//   - 髢｢謨ｰ蜻ｼ縺ｳ蜃ｺ縺玲凾・・nvokeFunction・・
//   - 繝悶Ο繝・け繧ｹ繧ｳ繝ｼ繝暦ｼ・f, while, for・・
//   - self 繧呈ｳｨ蜈･縺励◆繝｡繧ｽ繝・ラ蜻ｼ縺ｳ蜃ｺ縺・
//
// outer_ 縺ｫ隕ｪ繧ｹ繧ｳ繝ｼ繝励ｒ菫晄戟縺吶ｋ縺薙→縺ｧ縲√Ξ繧ｭ繧ｷ繧ｫ繝ｫ繧ｹ繧ｳ繝ｼ繝励ｒ螳溽樟縺吶ｋ縲・
// -----------------------------------------------------------------------------
Environment::Environment(std::shared_ptr<object::Environment> outer)
    : store_()
    , outer_(std::move(outer))
    , currentFunction_() {

}

// Get(name)
// -----------------------------------------------------------------------------
// 螟画焚蜷・name 繧堤樟蝨ｨ縺ｮ繧ｹ繧ｳ繝ｼ繝励°繧画､懃ｴ｢縺励∬ｦ九▽縺九ｉ縺ｪ縺代ｌ縺ｰ outer_ 繧定ｾｿ繧九・
// 縺薙ｌ縺ｯ Monkey 縺ｮ繝ｬ繧ｭ繧ｷ繧ｫ繝ｫ繧ｹ繧ｳ繝ｼ繝励→蜷後§謖吝虚縲・
// 
// tsumugi 縺ｧ縺ｯ縲《elf・医Γ繧ｽ繝・ラ蜻ｼ縺ｳ蜃ｺ縺玲凾縺ｮ receiver・峨ｂ Environment 縺ｫ
// 騾壼ｸｸ縺ｮ螟画焚縺ｨ縺励※譚溽ｸ帙＆繧後ｋ縺溘ａ縲；et("self") 縺ｧ蜿門ｾ励＆繧後ｋ縲・
// 
// 萓具ｼ・
//   let x = 10;
//   fn() { x }  // x 縺ｯ outer_ 繧定ｾｿ縺｣縺ｦ隕九▽縺九ｋ
//
//   obj.method() 縺ｮ蝣ｴ蜷茨ｼ・
//     InvokeFunction 縺梧眠縺励＞ Environment 繧剃ｽ懊ｊ縲・
//     縺昴％縺ｫ self 繧呈據邵帙☆繧九・
// -----------------------------------------------------------------------------
std::shared_ptr<object::IObject> Environment::Get(const tstring& name) {

    auto it = store_.find(name);
    if (it != store_.end()) {
        return it->second;
    }

    // 閾ｪ蛻・・繧ｹ繧ｳ繝ｼ繝励↓縺ｪ縺代ｌ縺ｰ隕ｪ繧呈爾縺励↓陦後￥
    if (auto outer = outer_.lock()) {
        return outer->Get(name);
    }

    return nullptr;
}

// Set(name, value)
// -----------------------------------------------------------------------------
// 迴ｾ蝨ｨ縺ｮ繧ｹ繧ｳ繝ｼ繝励↓ name 竊・value 縺ｮ譚溽ｸ帙ｒ霑ｽ蜉縺吶ｋ縲・
// Monkey 縺ｨ蜷後§縺上∽ｻ｣蜈･縺ｯ蟶ｸ縺ｫ縲檎樟蝨ｨ縺ｮ繧ｹ繧ｳ繝ｼ繝励阪↓蟇ｾ縺励※陦後ｏ繧後ｋ縲・
// 
// 萓具ｼ・
//   let x = 1;      // Set("x", 1)
//   fn() { x = 2 }  // x 縺ｯ outer_ 縺ｫ縺ゅｋ縺溘ａ縲・valAssignmentExpression 蛛ｴ縺ｧ
//                   // outer_ 繧定ｾｿ縺｣縺ｦ譖ｴ譁ｰ縺吶ｋ蠢・ｦ√′縺ゅｋ・・sumugi 蛛ｴ縺ｮ螳溯｣・ｬ｡隨ｬ・・
//
// tsumugi 縺ｧ縺ｯ setter・・bj.x = value・峨ｂ蟄伜惠縺吶ｋ縺溘ａ縲・
// 繝励Ο繝代ユ繧｣莉｣蜈･縺ｯ ObjectProtocolDispatcher 縺ｫ蟋碑ｭｲ縺輔ｌ繧九・
// Set 縺ｯ縲悟､画焚莉｣蜈･縲阪↓髯仙ｮ壹＆繧後ｋ縲・
// -----------------------------------------------------------------------------
std::shared_ptr<object::IObject> Environment::Set(const tstring& name, std::shared_ptr<IObject> value) {

    // 縺吶〒縺ｫ縺薙・繧ｹ繧ｳ繝ｼ繝励↓縺ゅｋ縺ｪ繧画峩譁ｰ
    if (store_.find(name) != store_.end()) {
        store_[name] = value;
        return value;
    }

    // 隕ｪ繧ｹ繧ｳ繝ｼ繝励↓縺ゅｋ縺ｪ繧芽ｦｪ繧呈峩譁ｰ
    if (auto outer = outer_.lock()) {
        if (outer->store_.find(name) != outer->store_.end()) {
            outer->Set(name, value);
            return value;
        }
    }

    store_[name] = value;
    return value;
}

// Clear()
// -----------------------------------------------------------------------------
// Environment 縺御ｿ晄戟縺吶ｋ螟画焚譚溽ｸ幢ｼ・tore_・峨ｒ遐ｴ譽・＠縲・
// FunctionObject 縺ｨ Environment 縺ｮ蠕ｪ迺ｰ蜿ら・繧定ｧ｣豸医☆繧九◆繧√・繝｡繧ｽ繝・ラ縲・
// 
// 縲舌↑縺懷ｿ・ｦ√°縲・
// UserFunctionObject 縺ｯ Environment 繧剃ｿ晄戟縺励・
// Environment 縺ｯ IObject・・serFunctionObject 繧貞性繧・峨ｒ菫晄戟縺吶ｋ縺溘ａ縲・
// 蠕ｪ迺ｰ蜿ら・縺檎匱逕溘☆繧句庄閭ｽ諤ｧ縺後≠繧九・
// 
// 騾壼ｸｸ縺ｮ螳溯｡後〒縺ｯ蝠城｡後↑縺・′縲ヽEPL 繧・せ繧ｯ繝ｪ繝励ヨ螳溯｡悟ｾ後↓ Environment 繧堤ｴ譽・☆繧矩圀縲・
// 蠕ｪ迺ｰ蜿ら・縺梧ｮ九ｋ縺ｨ繝｡繝｢繝ｪ繝ｪ繝ｼ繧ｯ縺ｨ縺励※讀懷・縺輔ｌ繧九・
// 
// Clear() 繧貞他縺ｶ縺薙→縺ｧ store_ 繧堤ｩｺ縺ｫ縺励・
// Environment 竊・IObject 竊・Environment 縺ｮ蠕ｪ迺ｰ繧呈妙縺｡蛻・ｋ縲・
// 
// 縲仙他縺ｶ縺ｹ縺阪ち繧､繝溘Φ繧ｰ縲・
// - REPL 邨ゆｺ・凾
// - 繧ｹ繧ｯ繝ｪ繝励ヨ螳溯｡悟ｾ後↓ Environment 繧堤ｴ譽・☆繧狗峩蜑・
// -----------------------------------------------------------------------------
void Environment::Clear() {

    store_.clear();
}

}

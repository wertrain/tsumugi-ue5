#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <optional>

namespace tsumugi::script::object::protocol {

// ObjectProtocolDispatcher
// -----------------------------------------------------------------------------
// Monkey 縺ｫ縺ｯ蟄伜惠縺励↑縺・》sumugi 迢ｬ閾ｪ縺ｮ繧ｪ繝悶ず繧ｧ繧ｯ繝医Δ繝・Ν繧呈髪縺医ｋ
// 縲後・繝ｭ繝代ユ繧｣繧｢繧ｯ繧ｻ繧ｹ縺ｮ邨ｱ荳繧､繝ｳ繧ｿ繝ｼ繝輔ぉ繝ｼ繧ｹ縲阪・
// 
// obj.x 繧・obj["x"] 縺ｮ繧医≧縺ｪ繝励Ο繝代ユ繧｣繧｢繧ｯ繧ｻ繧ｹ繧偵∝梛縺ｫ萓晏ｭ倥○縺・
// 荳蜈・噪縺ｫ謇ｱ縺・◆繧√・繝・ぅ繧ｹ繝代ャ繝√Ε縺ｧ縺ゅｋ縲・
//
// 縲仙ｽｹ蜑ｲ縲・
// - UserObject / 邨・∩霎ｼ縺ｿ蝙・/ 繧ｯ繝ｩ繧ｹ / 驟榊・ / 譁・ｭ怜・縺ｪ縺ｩ縲・
//   縺ゅｉ繧・ｋ繧ｪ繝悶ず繧ｧ繧ｯ繝医・繝励Ο繝代ユ繧｣蜿門ｾ励・險ｭ螳壹ｒ邨ｱ荳逧・↓謇ｱ縺・・
// - Evaluator 縺九ｉ蝙句・蟯舌ｒ謗帝勁縺励√が繝悶ず繧ｧ繧ｯ繝医＃縺ｨ縺ｮ繝励Ο繝医さ繝ｫ繧・
//   縺薙％縺ｫ髮・ｴ・☆繧九％縺ｨ縺ｧ縲∬ｨ隱槫・菴薙・荳雋ｫ諤ｧ縺ｨ諡｡蠑ｵ諤ｧ繧堤｢ｺ菫昴☆繧九・
//
// 縲腎sumugi 縺ｮ諢丞袖隲悶↓縺翫￠繧倶ｽ咲ｽｮ縺･縺代・
// - TryGetProperty 縺ｯ縲後・繝ｭ繝代ユ繧｣蛟､繧定ｿ斐☆縺縺代阪ｒ諡・ｽ薙＠縲・
//   繝｡繧ｽ繝・ラ蛹厄ｼ・his 縺ｮ豕ｨ蜈･・峨・陦後ｏ縺ｪ縺・・
//   竊・obj.foo() / super.foo() 縺ｮ繝ｬ繧ｷ繝ｼ繝先ｱｺ螳壹・
//      EvalCallExpression 竊・Invoke 縺ｫ髮・ｴ・＆繧後ｋ縲・
// - TrySetProperty 縺ｯ UserObject 縺ｪ縺ｩ縲後・繝ｭ繝代ユ繧｣繧呈戟縺､蝙九阪□縺代′蟇ｾ蠢懊＠縲・
//   邨・∩霎ｼ縺ｿ蝙九・騾壼ｸｸ縺ｯ險ｭ螳壻ｸ榊庄縲・
// - IsCallable 縺ｯ縲悟他縺ｳ蜃ｺ縺怜庄閭ｽ縺九←縺・°縲阪ｒ蛻､螳壹＠縲！nvoke 縺悟茜逕ｨ縺吶ｋ縲・
//
// 縲先署萓帶ｩ溯・縲・
// - TryGetProperty: 繝励Ο繝代ユ繧｣蜿門ｾ・
// - TrySetProperty: 繝励Ο繝代ユ繧｣險ｭ螳・
// - IsCallable: 蜻ｼ縺ｳ蜃ｺ縺怜庄閭ｽ繧ｪ繝悶ず繧ｧ繧ｯ繝医°縺ｩ縺・°縺ｮ蛻､螳・
//
// tsumugi 縺ｮ繧ｪ繝悶ず繧ｧ繧ｯ繝医Δ繝・Ν縺ｫ縺翫￠繧九後・繝ｭ繝代ユ繧｣繧｢繧ｯ繧ｻ繧ｹ縺ｮ荳ｭ譬ｸ縲阪→縺ｪ繧・
// 繧ｳ繝ｳ繝昴・繝阪Φ繝医〒縺ゅｊ縲・valuator 縺ｨ蜷・が繝悶ず繧ｧ繧ｯ繝亥梛縺ｮ讖区ｸ｡縺励ｒ陦後≧縲・
// -----------------------------------------------------------------------------

class ObjectProtocolDispatcher {
public:

    // 繝励Ο繝代ユ繧｣蜿門ｾ・
    // obj.x 縺ｮ隧穂ｾ｡譎ゅ↓蜻ｼ縺ｰ繧後ｋ縲・
    // TryGetProperty 縺ｯ縲後・繝ｭ繝代ユ繧｣蛟､繧貞叙蠕励☆繧九阪％縺ｨ縺縺代ｒ諡・ｽ薙☆繧・
    static std::optional<std::shared_ptr<object::IObject>> TryGetProperty(std::shared_ptr<object::IObject> object, const tstring& name);
    // 繝励Ο繝代ユ繧｣險ｭ螳・
    // obj.x = value 縺ｮ繧医≧縺ｪ莉｣蜈･縺ｫ蟇ｾ蠢懊☆繧九・
    // UserObject 縺ｮ繧医≧縺ｫ縲後・繝ｭ繝代ユ繧｣繧呈戟縺､蝙九阪・縺ｿ縺悟ｯｾ蠢懊＠縲∫ｵ・∩霎ｼ縺ｿ蝙具ｼ・rray, String 縺ｪ縺ｩ・峨・騾壼ｸｸ縺ｯ險ｭ螳壻ｸ榊庄縲・
    static bool TrySetProperty(std::shared_ptr<object::IObject> object, const tstring& name, std::shared_ptr<object::IObject> value);
    // 蜻ｼ縺ｳ蜃ｺ縺怜庄閭ｽ縺九←縺・°・磯未謨ｰ繝ｻ繝｡繧ｽ繝・ラ繝ｻ邨・∩霎ｼ縺ｿ髢｢謨ｰ縺ｪ縺ｩ・・
    // Invoke 縺悟他縺ｳ蜃ｺ縺怜燕縺ｫ蛻ｩ逕ｨ縺吶ｋ縲・
    static bool IsCallable(const std::shared_ptr<object::IObject>& object);
};

}

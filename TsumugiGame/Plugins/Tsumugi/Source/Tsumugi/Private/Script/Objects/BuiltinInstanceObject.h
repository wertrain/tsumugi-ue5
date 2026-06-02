#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include "Script/Builtins/BuiltinTypes.h"
#include <unordered_map>
#include <optional>

namespace tsumugi::script::object { class BuiltinClassObject; }

namespace tsumugi::script::object {

// BuiltinInstanceObject 縺ｯ縲檎ｵ・∩霎ｼ縺ｿ繧ｯ繝ｩ繧ｹ縺ｮ繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ縲阪ｒ陦ｨ縺吶・
// 
// 萓具ｼ・
//   let v = Vector3(1, 2, 3)
//   v.x
//   v.length()
// 
// 迚ｹ蠕ｴ・・
// - 蜍慕噪繝励Ο繝代ユ繧｣霎樊嶌・・roperties_・峨ｒ謖√▽
// - prototype 縺ｫ繧医ｋ繝｡繧ｽ繝・ラ謗｢邏｢繧偵し繝昴・繝医☆繧・
// - UserObject 縺ｨ莨ｼ縺ｦ縺・ｋ縺後《uper 繧・Θ繝ｼ繧ｶ繝ｼ螳夂ｾｩ繧ｯ繝ｩ繧ｹ縺ｨ縺ｯ辟｡髢｢菫・
// - Vector3, Transform, Color 縺ｪ縺ｩ縺ｮ縲檎ｵ・∩霎ｼ縺ｿ讒矩菴薙阪・繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ縺ｫ菴ｿ逕ｨ縺吶ｋ
// 
// 窶ｻ繝ｦ繝ｼ繧ｶ繝ｼ螳夂ｾｩ繧ｯ繝ｩ繧ｹ縺ｮ繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ縺ｫ縺ｯ UserObject 繧剃ｽｿ逕ｨ縺吶ｋ縲・
//   BuiltinInstanceObject 縺ｯ邨・∩霎ｼ縺ｿ蝙句ｰら畑縺ｧ縺ゅｊ縲・
//   邯呎価繝√ぉ繝ｼ繝ｳ繧・super 隗｣豎ｺ縺ｯ繧ｵ繝昴・繝医＠縺ｪ縺・・

class BuiltinInstanceObject : public IObject {
public:
    explicit BuiltinInstanceObject();

    /// <summary>
    /// 蝙句愛螳壹↓菴ｿ逕ｨ縺吶ｋ縺溘ａ縲∝ｿ・医→縺励∪縺・
    /// 豢ｾ逕溘け繝ｩ繧ｹ縺ｧ縺ｯ DEFINE_BUILTIN_CLASS 繝槭け繝ｭ繧剃ｽｿ逕ｨ縺励※螳夂ｾｩ縺励※縺上□縺輔＞
    /// </summary>
    /// <returns></returns>
    virtual builtin::BuiltinType GetBuiltinType() const { return builtin::BuiltinType::Num; }

    /// <summary>
    /// 閾ｪ霄ｫ縺ｮ繝励Ο繝代ユ繧｣縺九ｉ繝｡繝ｳ繝舌・繧貞叙蠕励＠縺ｾ縺・
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    std::shared_ptr<IObject> Get(const tstring& name) const;
    /// <summary>
    /// 閾ｪ霄ｫ縺ｮ繝励Ο繝代ユ繧｣縺ｫ繝｡繝ｳ繝舌・繧堤匳骭ｲ縺励∪縺・
    /// </summary>
    /// <param name="name"></param>
    /// <param name="value"></param>
    void Set(const tstring& name, std::shared_ptr<IObject> value);
    /// <summary>
    /// 繝励Ο繝医ち繧､繝励ｒ蜿門ｾ励＠縺ｾ縺・
    /// </summary>
    /// <returns></returns>
    std::shared_ptr<BuiltinInstanceObject> GetPrototype() const;
    /// <summary>
    /// 繝励Ο繝医ち繧､繝励ｒ險ｭ螳壹＠縺ｾ縺・
    /// </summary>
    /// <param name="proto"></param>
    void SetPrototype(std::shared_ptr<BuiltinInstanceObject> proto);

    tstring Inspect() const override;
    ObjectType GetType() const override;

    /// <summary>
    /// 繝励Ο繝代ユ繧｣繧貞叙蠕励＠縺ｾ縺・
    /// 閾ｪ霄ｫ縺ｮ繝励Ο繝代ユ繧｣縺ｨ繝励Ο繝医ち繧､繝励°繧画､懃ｴ｢縺励∪縺・
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    std::optional<std::shared_ptr<object::IObject>> TryGetProperty(const tstring& name);
    /// <summary>
    /// 閾ｪ霄ｫ縺ｮ繝励Ο繝代ユ繧｣縺ｫ繝励Ο繝代ユ繧｣縺ｫ蛟､繧定ｨｭ螳壹＠縺ｾ縺・
    /// 縺薙・繝｡繧ｽ繝・ラ繧剃ｽｿ逕ｨ縺励◆蝣ｴ蜷医∝梛縺ｯ蝠上ｏ縺壹↓險ｭ螳壹〒縺阪ｋ縺溘ａ
    /// 蝓ｺ譛ｬ逧・↓縺ｯ豢ｾ逕溷・縺ｧ諢丞峙縺励◆蝙九°繝√ぉ繝・け縲√ｂ縺励￥縺ｯ螟画鋤縺励※繧ｻ繝・ヨ縺吶ｋ縺薙→繧呈耳螂ｨ縺励∪縺・
    /// </summary>
    /// <param name="name"></param>
    /// <param name="value"></param>
    /// <returns></returns>
    virtual bool TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value);

private:
    std::unordered_map<tstring, std::shared_ptr<IObject>> properties_;
    std::shared_ptr<BuiltinInstanceObject> prototype_;
};

#define DEFINE_BUILTIN_CLASS(builtinType) \
    static inline constexpr builtin::BuiltinType StaticType = builtinType; \
    builtin::BuiltinType GetBuiltinType() const override { return StaticType; } \

template <builtin::BuiltinType T>
bool IsInstanceOf(const std::shared_ptr<IObject>& obj) {
    if (!obj) return false;
    if (obj->GetType() != ObjectType::kBuiltinInstance) return false;

    auto inst = std::static_pointer_cast<BuiltinInstanceObject>(obj);
    return inst->GetBuiltinType() == T;
}

}

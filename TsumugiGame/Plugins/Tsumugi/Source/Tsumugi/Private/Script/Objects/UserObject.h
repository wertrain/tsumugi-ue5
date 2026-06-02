#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <unordered_map>
#include <optional>

namespace tsumugi::script::object { class ClassObject; }

namespace tsumugi::script::object {

// UserObject 縺ｯ縲悟虚逧・・繝ｭ繝代ユ繧｣繧呈戟縺､繧ｪ繝悶ず繧ｧ繧ｯ繝医阪ｒ陦ｨ縺吶・
// 
// 逕ｨ騾斐・2縺､・・
// 1. 繝ｦ繝ｼ繧ｶ繝ｼ螳夂ｾｩ繧ｯ繝ｩ繧ｹ縺ｮ繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ
//      class Foo {} 竊・Foo() 縺ｧ逕滓・縺輔ｌ繧九う繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ
// 2. 繧ｪ繝悶ず繧ｧ繧ｯ繝医Μ繝・Λ繝ｫ
//      let o = { x: 0 } 縺ｮ繧医≧縺ｪ繝ｪ繝・Λ繝ｫ繧・UserObject 縺ｫ縺ｪ繧・
//
// prototype_ 縺ｫ繧医ｋ繝励Ο繝医ち繧､繝励メ繧ｧ繝ｼ繝ｳ繧呈戟縺｡縲・
// JavaScript 縺ｮ Object 縺ｨ蜷梧ｧ倥・謖ｯ繧玖・縺・ｒ縺吶ｋ縲・
// 
// 窶ｻ豕ｨ諢擾ｼ啖ector2, Color 縺ｪ縺ｩ縺ｮ縲檎ｵ・∩霎ｼ縺ｿ讒矩菴薙け繝ｩ繧ｹ縲阪↓縺ｯ菴ｿ逕ｨ縺励↑縺・・
//   縺昴ｌ繧峨・ BuiltinInstanceObject 繧剃ｽｿ縺・・
//   UserObject 縺ｯ蜍慕噪繝励Ο繝代ユ繧｣霎樊嶌繧呈戟縺､縺溘ａ縲・
//   繝阪う繝・ぅ繝門､繧呈戟縺､邨・∩霎ｼ縺ｿ蝙九↓縺ｯ驕ｩ縺輔↑縺・・

class UserObject : public IObject {
public:
    explicit UserObject();

    std::shared_ptr<IObject> Get(const tstring& name) const;
    void Set(const tstring& name, std::shared_ptr<IObject> value);
    std::shared_ptr<UserObject> GetPrototype() const;
    void SetPrototype(std::shared_ptr<UserObject> proto);
    const std::unordered_map<tstring, std::shared_ptr<IObject>>& GetProperties() const { return properties_; }

    tstring Inspect() const override;
    ObjectType GetType() const override;

    std::optional<std::shared_ptr<object::IObject>> TryGetProperty(const tstring& name);
    std::shared_ptr<ClassObject> GetOwnerClass() const { return ownerClass_.lock(); }
    void SetOwnerClass(const std::shared_ptr<ClassObject>& klass) { ownerClass_ = klass; }

private:
    std::unordered_map<tstring, std::shared_ptr<IObject>> properties_;
    std::shared_ptr<UserObject> prototype_;
    std::weak_ptr<ClassObject> ownerClass_;
};

}

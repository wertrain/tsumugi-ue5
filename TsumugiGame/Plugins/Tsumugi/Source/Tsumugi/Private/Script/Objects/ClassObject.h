#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <unordered_map>
#include <optional>

namespace tsumugi::script::object { class UserObject; }

namespace tsumugi::script::object {

// ClassObject 縺ｯ縲後Θ繝ｼ繧ｶ繝ｼ螳夂ｾｩ繧ｯ繝ｩ繧ｹ縲阪ｒ陦ｨ縺吶が繝悶ず繧ｧ繧ｯ繝医・
// 
// - `class Foo {}` 繧貞ｮ夂ｾｩ縺吶ｋ縺ｨ ClassObject 縺檎函謌舌＆繧後ｋ縲・
// - `Foo()` 繧貞他縺ｶ縺ｨ UserObject・医う繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ・峨′逕滓・縺輔ｌ繧九・
// - prototype 繝吶・繧ｹ縺ｮ繝｡繧ｽ繝・ラ隗｣豎ｺ繧・ｶ呎価・・arent_・峨ｒ繧ｵ繝昴・繝医☆繧九・
// 
// 窶ｻ豕ｨ諢擾ｼ啌andom, Math, Vector2 縺ｪ縺ｩ縺ｮ縲檎ｵ・∩霎ｼ縺ｿ繧ｯ繝ｩ繧ｹ縲阪↓縺ｯ菴ｿ逕ｨ縺励↑縺・・
//   邨・∩霎ｼ縺ｿ繧ｯ繝ｩ繧ｹ縺ｯ BuiltinClassObject / BuiltinInstanceObject 繧剃ｽｿ縺・・
//   UserObject 縺ｯ繝ｦ繝ｼ繧ｶ繝ｼ螳夂ｾｩ繧ｯ繝ｩ繧ｹ蟆ら畑縺ｮ繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ陦ｨ迴ｾ縺ｧ縺ゅｊ縲・
//   邨・∩霎ｼ縺ｿ讒矩菴難ｼ・ector2 縺ｪ縺ｩ・峨↓縺ｯ驕ｩ縺輔↑縺・・

// prototype_ 縺ｯ繝ｦ繝ｼ繧ｶ繝ｼ螳夂ｾｩ繧ｯ繝ｩ繧ｹ縺ｮ繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ・・serObject・峨′
// 繝｡繧ｽ繝・ラ繧呈爾邏｢縺吶ｋ髫帙・繝励Ο繝医ち繧､繝励メ繧ｧ繝ｼ繝ｳ縺ｮ襍ｷ轤ｹ縺ｨ縺ｪ繧九・
// JavaScript 縺ｮ Foo.prototype 縺ｫ逶ｸ蠖薙☆繧九・

class ClassObject : public IObject {
public:
    static constexpr const tchar* kConstructorName = TT("init");
    static constexpr const tchar* kSuperKeyword = TT("super");

public:
    ClassObject(const tstring& name, std::shared_ptr<UserObject> prototype, std::shared_ptr<ClassObject> parent);
    ClassObject(const tstring& name);

    const tstring& GetName() const { return name_; }
    std::shared_ptr<UserObject> GetPrototype() const { return prototype_; }
    std::shared_ptr<ClassObject> GetParentClass() const { return parent_; }

    void SetPrototype(std::shared_ptr<UserObject> prototype) { prototype_ = std::move(prototype); }
    void SetParent(std::shared_ptr<ClassObject> parent) { parent_ = std::move(parent); }
    void SetStaticMethod(const tstring& name, std::shared_ptr<IObject> fn) { staticMethods_[name] = fn; }

    std::optional<std::shared_ptr<IObject>> TryGetOwnMethod(const tstring& name) const;
    std::optional<std::shared_ptr<IObject>> TryGetMethod(const tstring& name) const;
    std::optional<std::shared_ptr<IObject>> TryGetStaticMethod(const tstring& name) const;
    std::optional<std::shared_ptr<IObject>> TryGetProperty(const tstring& name) const;
    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    tstring name_;
    std::unordered_map<tstring, std::shared_ptr<IObject>> staticMethods_;
    std::shared_ptr<UserObject> prototype_;
    std::shared_ptr<ClassObject> parent_;
};

}

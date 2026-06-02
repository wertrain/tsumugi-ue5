#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <unordered_map>
#include <optional>
#include <functional>

namespace tsumugi::script::object { class BuiltinInstanceObject; }

namespace tsumugi::script::object {

// BuiltinClassObject 縺ｯ縲檎ｵ・∩霎ｼ縺ｿ繧ｯ繝ｩ繧ｹ縲阪ｒ陦ｨ縺吶が繝悶ず繧ｧ繧ｯ繝医・
// 
// - Random, Math 縺ｮ繧医≧縺ｪ static-only 繧ｯ繝ｩ繧ｹ
// - Vector3, Transform 縺ｮ繧医≧縺ｪ instance-based 繧ｯ繝ｩ繧ｹ
// 
// 縺ｮ荳｡譁ｹ繧呈桶縺医ｋ繧医≧縺ｫ險ｭ險医＆繧後※縺・ｋ縲・
// 
// 縲尽tatic-only 繝｢繝ｼ繝峨・
//   - prototype 繧呈戟縺溘↑縺・
//   - 繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ逕滓・荳榊庄・・andom(), Math() 縺ｯ繧ｨ繝ｩ繝ｼ・・
//   - static 繝｡繧ｽ繝・ラ縺ｮ縺ｿ繧呈署萓帙☆繧・
// 
// 縲進nstance-based 繝｢繝ｼ繝峨・
//   - prototype・・uiltinInstanceObject・峨ｒ謖√▽
//   - 繧ｯ繝ｩ繧ｹ蜻ｼ縺ｳ蜃ｺ縺励〒繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ繧堤函謌舌〒縺阪ｋ・・ector3(1,2,3)・・
//   - instance 繝｡繧ｽ繝・ラ縺ｨ static 繝｡繧ｽ繝・ラ縺ｮ荳｡譁ｹ繧呈戟縺ｦ繧・
// 
// 窶ｻ繝ｦ繝ｼ繧ｶ繝ｼ螳夂ｾｩ繧ｯ繝ｩ繧ｹ・・lass Foo {}・峨↓縺ｯ ClassObject 繧剃ｽｿ逕ｨ縺吶ｋ縲・
//   BuiltinClassObject 縺ｯ邯呎価繝√ぉ繝ｼ繝ｳ縺ｫ縺ｯ蜿ょ刈縺励↑縺・ｼ育樟迥ｶ縺ｮ莉墓ｧ假ｼ峨・
//   蟆・擂逧・↓邨・∩霎ｼ縺ｿ繧ｯ繝ｩ繧ｹ縺ｮ邯呎価繧偵し繝昴・繝医☆繧句ｴ蜷医・縲・
//   ClassObject 縺ｨ讒矩繧堤ｵｱ蜷医☆繧句ｿ・ｦ√′縺ゅｋ縲・

class BuiltinClassObject : public IObject {
public:
    BuiltinClassObject(const tstring_view& name);

    const tstring& GetName() const { return name_; }
    void SetStaticMethod(const tstring& name, std::shared_ptr<IObject> fn) { staticMethods_[name] = fn; }
    void SetInstanceMethod(const tstring& name, std::shared_ptr<IObject> fn);
    void SetInstanceCreator(std::function<std::shared_ptr<IObject>(const std::vector<std::shared_ptr<IObject>>&)> creator);
    bool CanInstantiate() const;
    std::shared_ptr<BuiltinInstanceObject> GetPrototype() const { return prototype_; }
    std::optional<std::shared_ptr<IObject>> Instantiate(const std::vector<std::shared_ptr<IObject>>& arguments) const;
    std::optional<std::shared_ptr<IObject>> TryGetProperty(const tstring& name) const;
    tstring Inspect() const override;
    ObjectType GetType() const override;

public:
    // 豢ｾ逕溷・縺ｮ繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ逕滓・縺ｫ菴ｿ逕ｨ縺吶ｋ・井ｸｻ縺ｫ遒ｺ螳溘↓ SetPrototype 縺輔○繧九◆繧√・繧ゅ・・・
    //template <typename T = object::BuiltinInstanceObject, typename... Args>
    //std::shared_ptr<T> CreateInstance(Args&&... args) const {
    //    auto instance = std::make_shared<T>(std::forward<Args>(args)...);
    //    if (prototype_) {
    //        instance->SetPrototype(prototype_);
    //    }
    //    return instance;
    //}

private:
    void EnsurePrototype();

private:
    tstring name_;
    std::unordered_map<tstring, std::shared_ptr<IObject>> staticMethods_;
    std::shared_ptr<BuiltinInstanceObject> prototype_; // null 縺ｮ蝣ｴ蜷医・ static-only 繝｢繝ｼ繝・
    std::function<std::shared_ptr<IObject>(const std::vector<std::shared_ptr<IObject>>&)> instanceCreator_;
};

}

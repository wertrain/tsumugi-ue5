#include "Script/Builtins/Transform/TransformClass.h"
#include "Script/Builtins/Transform/TransformInstance.h"
#include "Script/Builtins/BuiltinClassRegistry.h"
#include "Script/Builtins/Vector/Vector3Instance.h"
#include "Script/Builtins/Quaternion/QuaternionInstance.h"
#include "Script/Objects/BuiltinClassObject.h"
#include "Script/Objects/BuiltinInstanceObject.h"
#include "Script/Objects/BuiltinFunctionObject.h"
#include "Script/Objects/FloatObject.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/BooleanObject.h"
#include "Script/Objects/NullObject.h"

namespace tsumugi::script::builtin::transform {

std::shared_ptr<object::BuiltinClassObject> CreateTransformClass() {

    auto klass = std::make_shared<object::BuiltinClassObject>(builtin::BuiltinTypeName(builtin::BuiltinType::Transform));

    // ラムダ内での循環参照を防ぐために weak_ptr を作成
    std::weak_ptr<object::BuiltinClassObject> weakClass = klass;

    //
    // --- instance メソッド ---
    //

    //
    // --- static メソッド ---
    //

    //
    // identity()
    //
    klass->SetStaticMethod(
        TT("identity"),
        std::make_shared<object::BuiltinFunctionObject>(
            [weakClass](std::shared_ptr<object::IObject>,
                const std::vector<std::shared_ptr<object::IObject>>&)
            -> std::shared_ptr<object::IObject>
            {
                return BuiltinClassRegistry::CreateInstance<TransformInstance>();
            }
        )
    );

    return klass;
}

}

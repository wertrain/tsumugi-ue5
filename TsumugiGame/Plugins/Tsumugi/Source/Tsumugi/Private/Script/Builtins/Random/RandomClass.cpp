#include "Script/Builtins/Random/RandomClass.h"
#include "Script/Builtins/Random/RandomInstance.h"
#include "Script/Objects/BuiltinClassObject.h"
#include "Script/Objects/BuiltinFunctionObject.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/FloatObject.h"
#include "Script/Objects/NullObject.h"
#include "Script/Objects/ArrayObject.h"
#include <random>

namespace tsumugi::script::builtins::random {

static std::mt19937 g_rng(std::random_device{}());

std::shared_ptr<object::BuiltinClassObject> CreateRandomClass() {

    auto klass = std::make_shared<object::BuiltinClassObject>(RandomInstance::StaticClassName);

    // ラムダ内での循環参照を防ぐために weak_ptr を作成
    //std::weak_ptr<object::BuiltinClassObject> weakClass = klass;

    //
    // --- インスタンス生成 ---
    //
    //klass->SetInstanceCreator(
    //    [weakClass](const std::vector<std::shared_ptr<object::IObject>>& args)
    //    -> std::shared_ptr<object::IObject>
    //    {
    //        if (auto classPtr = weakClass.lock()) {
    //            return classPtr->CreateInstance<RandomInstance>();
    //        }
    //        return nullptr;
    //    }
    //);

    //
    // --- instance メソッド ---
    //

    //
    // --- static メソッド ---
    //

    //
    // --- int(min, max) ---
    //
    klass->SetStaticMethod(
        TT("int"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject>, const std::vector<std::shared_ptr<object::IObject>>& args)
            -> std::shared_ptr<object::IObject>
            {
                if (args.size() < 2 ||
                    args[0]->GetType() != object::ObjectType::kInteger ||
                    args[1]->GetType() != object::ObjectType::kInteger)
                {
                    return object::NullObject::Instance();
                }

                int min = std::static_pointer_cast<object::IntegerObject>(args[0])->GetValue();
                int max = std::static_pointer_cast<object::IntegerObject>(args[1])->GetValue();

                if (max < min) std::swap(min, max);

                std::uniform_int_distribution<int> dist(min, max);
                return std::make_shared<object::IntegerObject>(dist(g_rng));
            }
        )
    );

    //
    // --- float() ---
    //
    klass->SetStaticMethod(
        TT("float"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject>, const std::vector<std::shared_ptr<object::IObject>>& args)
            -> std::shared_ptr<object::IObject>
            {
                std::uniform_real_distribution<double> dist(0.0, 1.0);
                return std::make_shared<object::FloatObject>(dist(g_rng));
            }
        )
    );

    //
    // --- range(n) ---
    //
    klass->SetStaticMethod(
        TT("range"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject>, const std::vector<std::shared_ptr<object::IObject>>& args)
            -> std::shared_ptr<object::IObject>
            {
                if (args.size() < 1 ||
                    args[0]->GetType() != object::ObjectType::kInteger)
                {
                    return object::NullObject::Instance();
                }

                int n = std::static_pointer_cast<object::IntegerObject>(args[0])->GetValue();
                if (n <= 0) return object::NullObject::Instance();

                std::uniform_int_distribution<int> dist(0, n - 1);
                return std::make_shared<object::IntegerObject>(dist(g_rng));
            }
        )
    );

    //
    // --- choice(array) ---
    //
    klass->SetStaticMethod(
        TT("choice"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject>, const std::vector<std::shared_ptr<object::IObject>>& args)
            -> std::shared_ptr<object::IObject>
            {
                if (args.size() < 1 ||
                    args[0]->GetType() != object::ObjectType::kArray)
                {
                    return object::NullObject::Instance();
                }

                auto arr = std::static_pointer_cast<object::ArrayObject>(args[0]);
                auto& elems = arr->GetElements();

                if (elems.empty()) return object::NullObject::Instance();

                std::uniform_int_distribution<int> dist(0, elems.size() - 1);
                return elems[dist(g_rng)];
            }
        )
    );

    //
    // --- seed(value) ---
    //
    klass->SetStaticMethod(
        TT("seed"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject>, const std::vector<std::shared_ptr<object::IObject>>& args)
            -> std::shared_ptr<object::IObject>
            {
                if (args.size() < 1 ||
                    args[0]->GetType() != object::ObjectType::kInteger)
                {
                    return object::NullObject::Instance();
                }

                int seed = std::static_pointer_cast<object::IntegerObject>(args[0])->GetValue();
                g_rng.seed(seed);

                return object::NullObject::Instance();
            }
        )
    );

    return klass;
}

}

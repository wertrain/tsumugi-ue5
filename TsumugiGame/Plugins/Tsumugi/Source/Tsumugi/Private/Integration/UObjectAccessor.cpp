#include "Integration/UObjectAccessor.h"

#include "Integration/StringConversion.h"
#include "TsumugiEngine/Script/Objects/IntegerObject.h"
#include "TsumugiEngine/Script/Objects/FloatObject.h"
#include "TsumugiEngine/Script/Objects/StringObject.h"
#include "TsumugiEngine/Script/Objects/NullObject.h"
#include "TsumugiEngine/Script/Objects/ErrorObject.h"
#include "TsumugiEngine/Script/Objects/BuiltinFunctionObject.h"
#include "TsumugiEngine/Script/Objects/BuiltinInstanceObject.h"
#include "TsumugiEngine/Script/Builtins/Vector/Vector3Instance.h"
#include "TsumugiEngine/Script/Builtins/Vector/Vector2Instance.h"
#include "TsumugiEngine/Script/Builtins/Quaternion/QuaternionInstance.h"
#include "TsumugiEngine/Script/Builtins/Transform/TransformInstance.h"

namespace tsumugi::integration {

UObjectAccessor::UObjectAccessor(UObject* InTarget)
    : Target(InTarget)
{

}

std::shared_ptr<script::object::IObject> UObjectAccessor::Get(const tstring& name) const
{
    if (auto v = UserObject::Get(name))
    {
        return v;
    }


    if (auto v = GetUEProperty(name))
    {
        return v;
    }

    if (auto v = GetUFunction(name))
    {
        return v;
    }

    if (auto proto = GetPrototype())
    {
        return proto->Get(name);
    }

    return nullptr;
}

void UObjectAccessor::Set(const tstring& name, std::shared_ptr<script::object::IObject> value)
{
    if (SetUEProperty(name, value))
    {
        return;
    }

    // それ以外は UserObject の辞書に入れる
    UserObject::Set(name, value);
}

std::shared_ptr<script::object::IObject> UObjectAccessor::GetUEProperty(const tstring& name) const
{
    if (!Target) return nullptr;

    FProperty* Property = Target->GetClass()->FindPropertyByName(ToFName(name));
    if (!Property) return nullptr;

    uint8* Data = Property->ContainerPtrToValuePtr<uint8>(Target);

    if (auto* NumProperty = CastField<FIntProperty>(Property))
    {
        return std::make_shared<tsumugi::script::object::IntegerObject>(NumProperty->GetPropertyValue(Data));
    }

    if (auto* FloatProperty = CastField<FFloatProperty>(Property))
    {
        return std::make_shared<tsumugi::script::object::FloatObject>(FloatProperty->GetPropertyValue(Data));
    }

    if (auto* StrProperty = CastField<FStrProperty>(Property))
    {
        return std::make_shared<tsumugi::script::object::StringObject>(tsumugi::integration::ToTString(StrProperty->GetPropertyValue(Data)));
    }

    return nullptr;
}


bool UObjectAccessor::SetUEProperty(const tstring& name, const std::shared_ptr<script::object::IObject>& value)
{
    if (!Target) return false;

    FProperty* Property = Target->GetClass()->FindPropertyByName(ToFName(name));
    if (!Property) return false;

    uint8* Data = Property->ContainerPtrToValuePtr<uint8>(Target);

    if (auto* NumProp = CastField<FIntProperty>(Property))
    {
        if (value->GetType() == script::object::ObjectType::kInteger)
        {
            auto IntValue = std::static_pointer_cast<tsumugi::script::object::IntegerObject>(value);
            NumProp->SetPropertyValue(Data, IntValue->GetValue());
            return true;
        }
        else if (value->GetType() == script::object::ObjectType::kFloat)
        {
            auto FloatValue = std::static_pointer_cast<tsumugi::script::object::FloatObject>(value);
            NumProp->SetPropertyValue(Data, FloatValue->GetValue());
            return true;
        }
    }

    if (auto* FloatProp = CastField<FFloatProperty>(Property))
    {
        if (value->GetType() == script::object::ObjectType::kInteger)
        {
            auto IntValue = std::static_pointer_cast<tsumugi::script::object::IntegerObject>(value);
            FloatProp->SetPropertyValue(Data, IntValue->GetValue());
            return true;
        }
        else if (value->GetType() == script::object::ObjectType::kFloat)
        {
            auto FloatValue = std::static_pointer_cast<tsumugi::script::object::FloatObject>(value);
            FloatProp->SetPropertyValue(Data, FloatValue->GetValue());
            return true;
        }
    }

    if (auto* StrProp = CastField<FStrProperty>(Property))
    {
        if (value->GetType() == script::object::ObjectType::kString)
        {
            auto StringValue = std::static_pointer_cast<tsumugi::script::object::StringObject>(value);
            StrProp->SetPropertyValue(Data, tsumugi::integration::ToFString(StringValue->GetValue()));
            return true;
        }
    }

    return false;
}

std::shared_ptr<tsumugi::script::object::IObject> UObjectAccessor::GetUFunction(const tstring& name) const
{
    if (!Target) return nullptr;

    UFunction* Func = Target->FindFunction(ToFName(name));
    if (!Func) return nullptr;

    // ここで BuiltinFunctionObject にラップして返す
    // （あなたの BuiltinFunctionObject 実装に合わせて書き換え）
    return std::make_shared<tsumugi::script::object::BuiltinFunctionObject>(
        [this, Func](std::shared_ptr<tsumugi::script::object::IObject>, const std::vector<std::shared_ptr<tsumugi::script::object::IObject>>& args)
        -> std::shared_ptr<tsumugi::script::object::IObject>
        {
            uint8* Buffer = (uint8*)FMemory::Malloc(Func->ParmsSize);
            FMemory::Memzero(Buffer, Func->ParmsSize);

            int ArgIndex = 0;

            // すべての引数を処理する
            for (TFieldIterator<FProperty> It(Func); It && (It->PropertyFlags & CPF_Parm); ++It)
            {
                FProperty* Property = *It;
                uint8* PropertyData = Property->ContainerPtrToValuePtr<uint8>(Buffer);

                // ReturnParam はスキップ（後で読む）
                if (Property->HasAnyPropertyFlags(CPF_ReturnParm))
                    continue;

                if (ArgIndex >= args.size())
                    break;

                auto Arg = args[ArgIndex++];
                if (!ConvertArgument(Property, PropertyData, Arg))
                {
                    FMemory::Free(Buffer);
                    return std::make_shared<tsumugi::script::object::ErrorObject>(tsumugi::script::object::ErrorCode::kTypeMismatch, std::unordered_map<std::string, tstring>{});
                }

            }

            Target->ProcessEvent(Func, Buffer);

            std::shared_ptr<tsumugi::script::object::IObject> ReturnValue = tsumugi::script::object::NullObject::Instance();

            for (TFieldIterator<FProperty> It(Func); It; ++It)
            {
                FProperty* Property = *It;
                if (Property->HasAnyPropertyFlags(CPF_ReturnParm))
                {
                    uint8* PropertyData = Property->ContainerPtrToValuePtr<uint8>(Buffer);
                    ReturnValue = ConvertReturnValue(Property, PropertyData);
                    break;
                }
            }

            FMemory::Free(Buffer);

            return ReturnValue;
        }
    );
}

bool UObjectAccessor::ConvertArgument(class FProperty* Property, uint8* PropertyData, std::shared_ptr<script::object::IObject> Arg) const
{
    if (auto* FloatProperty = CastField<FFloatProperty>(Property))
    {
        if (Arg->GetType() == script::object::ObjectType::kInteger)
        {
            auto IntValue = std::static_pointer_cast<tsumugi::script::object::IntegerObject>(Arg);
            FloatProperty->SetPropertyValue(PropertyData, IntValue->GetValue());
            return true;
        }
        else if (Arg->GetType() == script::object::ObjectType::kFloat)
        {
            auto FloatValue = std::static_pointer_cast<tsumugi::script::object::FloatObject>(Arg);
            FloatProperty->SetPropertyValue(PropertyData, FloatValue->GetValue());
            return true;
        }
    }

    if (auto* IntProperty = CastField<FIntProperty>(Property))
    {
        if (Arg->GetType() == script::object::ObjectType::kInteger)
        {
            auto IntValue = std::static_pointer_cast<tsumugi::script::object::IntegerObject>(Arg);
            IntProperty->SetPropertyValue(PropertyData, IntValue->GetValue());
            return true;
        }
        else if (Arg->GetType() == script::object::ObjectType::kFloat)
        {
            auto FloatValue = std::static_pointer_cast<tsumugi::script::object::FloatObject>(Arg);
            IntProperty->SetPropertyValue(PropertyData, FloatValue->GetValue());
            return true;
        }
    }

    if (auto* StringProperty = CastField<FStrProperty>(Property))
    {
        if (Arg->GetType() == script::object::ObjectType::kString)
        {
            auto StringValue = std::static_pointer_cast<tsumugi::script::object::StringObject>(Arg);
            StringProperty->SetPropertyValue(PropertyData, tsumugi::integration::ToFString(StringValue->GetValue()));
            return true;
        }
    }

    if (auto* StructProperty = CastField<FStructProperty>(Property))
    {
        if (Arg->GetType() != script::object::ObjectType::kBuiltinInstance)
            return false;

        auto BuiltinInstanceValue = std::static_pointer_cast<tsumugi::script::object::BuiltinInstanceObject>(Arg);

        if (StructProperty->Struct == TBaseStructure<FVector>::Get())
        {
            if (BuiltinInstanceValue->GetBuiltinType() == tsumugi::script::builtin::BuiltinType::Vector3)
            {
                auto Vector3 = std::static_pointer_cast<tsumugi::script::builtin::vector::Vector3Instance>(BuiltinInstanceValue);
                *(FVector*)PropertyData = FVector(Vector3->X(), Vector3->Y(), Vector3->Z());
                return true;
            }
            // 暗黙の型変換しすぎ？
            else if (BuiltinInstanceValue->GetBuiltinType() == tsumugi::script::builtin::BuiltinType::Vector2)
            {
                auto Vector2 = std::static_pointer_cast<tsumugi::script::builtin::vector::Vector2Instance>(BuiltinInstanceValue);
                *(FVector*)PropertyData = FVector(Vector2->X(), Vector2->Y(), 0);
                return true;
            }
        }
        else if (StructProperty->Struct == TBaseStructure<FTransform>::Get())
        {
            if (BuiltinInstanceValue->GetBuiltinType() == tsumugi::script::builtin::BuiltinType::Transform)
            {
                auto Transform = std::static_pointer_cast<tsumugi::script::builtin::transform::TransformInstance>(BuiltinInstanceValue);

                const math::Vector3& TPosition = Transform->GetPosition();
                const math::Quaternion& TRotation = Transform->GetRotation();
                const math::Vector3& TScale = Transform->GetScale();

                FVector Position = FVector(TPosition.x, TPosition.y, TPosition.z);
                FQuat Rotation = FQuat(TRotation.x, TRotation.y, TRotation.z, TRotation.w);
                FVector Scale = FVector(TScale.x, TScale.y, TScale.z);

                *(FTransform*)PropertyData = FTransform(Rotation, Position, Scale);
                return true;
            }
        }
        // TODO: FRotator, FColor, FTransform なども追加
    }

    return false;
}

std::shared_ptr<script::object::IObject> UObjectAccessor::ConvertReturnValue(class FProperty* Property, uint8* PropertyData) const
{
    if (auto* FloatProperty = CastField<FFloatProperty>(Property))
    {
        const double Value = FloatProperty->GetPropertyValue(PropertyData);
        return std::make_shared<tsumugi::script::object::FloatObject>(Value);
    }

    if (auto* IntProperty = CastField<FIntProperty>(Property))
    {
        const int Value = IntProperty->GetPropertyValue(PropertyData);
        return std::make_shared<tsumugi::script::object::IntegerObject>(Value);
    }

    if (auto* StringProperty = CastField<FStrProperty>(Property))
    {
        const FString Value = StringProperty->GetPropertyValue(PropertyData);
        return std::make_shared<tsumugi::script::object::StringObject>(tsumugi::integration::ToTString(Value));
    }

    if (auto* StructProperty = CastField<FStructProperty>(Property))
    {
        if (StructProperty->Struct == TBaseStructure<FVector>::Get())
        {
            FVector Value = *(FVector*)PropertyData;
            return std::make_shared<tsumugi::script::builtin::vector::Vector3Instance>(Value.X, Value.Y, Value.Z);
        }
    }

    return tsumugi::script::object::NullObject::Instance();
    //return std::make_shared<tsumugi::script::object::ErrorObject>(tsumugi::script::object::ErrorCode::kInvalidProperty, std::unordered_map<std::string, tstring>{});
}

} // namespace tsumugi::integration

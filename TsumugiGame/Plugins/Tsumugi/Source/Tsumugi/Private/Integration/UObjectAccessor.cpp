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
#include "TsumugiEngine/Script/Builtins/Rotator/RotatorInstance.h"
#include "TsumugiEngine/Script/Builtins/Color/ColorInstance.h"
#include "TsumugiEngine/Script/Builtins/Color/LinearColorInstance.h"

namespace tsumugi::integration {

UObjectAccessor::UObjectAccessor(UObject* InTarget)
    : Target(InTarget)
{

}

std::optional<std::shared_ptr<script::object::IObject>> UObjectAccessor::TryGetProperty(const tstring& name) const
{
    if (auto v = UserObject::TryGetProperty(name))
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

    return std::nullopt;
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

    return ConvertPropertyValue(Property, Data);
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

    if (auto* StructProp = CastField<FStructProperty>(Property))
    {
        if (value->GetType() == script::object::ObjectType::kBuiltinInstance)
        {
            auto inst = std::static_pointer_cast<tsumugi::script::object::BuiltinInstanceObject>(value);

            // math::Color → FColor
            if (StructProp->Struct == TBaseStructure<FColor>::Get())
            {
                auto c = std::static_pointer_cast<tsumugi::script::builtin::color::ColorInstance>(value)->GetValue();

                FColor* Dest = reinterpret_cast<FColor*>(Data);
                Dest->R = c.r;
                Dest->G = c.g;
                Dest->B = c.b;
                Dest->A = c.a;

                return true;
            }

            // math::LinearColor → FLinearColor
            if (StructProp->Struct == TBaseStructure<FLinearColor>::Get())
            {
                auto c = std::static_pointer_cast<tsumugi::script::builtin::color::LinearColorInstance>(value)->GetValue();

                FLinearColor* Dest = reinterpret_cast<FLinearColor*>(Data);
                Dest->R = c.r;
                Dest->G = c.g;
                Dest->B = c.b;
                Dest->A = c.a;

                return true;
            }

            // math::Vector3 → FVector
            if (StructProp->Struct == TBaseStructure<FVector>::Get())
            {
                auto v = std::static_pointer_cast<tsumugi::script::builtin::vector::Vector3Instance>(value)->GetValue();

                FVector* Dest = reinterpret_cast<FVector*>(Data);
                Dest->X = v.x;
                Dest->Y = v.y;
                Dest->Z = v.z;

                return true;
            }

            // math::Quaternion → FQuat
            if (StructProp->Struct == TBaseStructure<FQuat>::Get())
            {
                auto q = std::static_pointer_cast<tsumugi::script::builtin::quaternion::QuaternionInstance>(value)->GetValue();

                FQuat* Dest = reinterpret_cast<FQuat*>(Data);
                Dest->X = q.x;
                Dest->Y = q.y;
                Dest->Z = q.z;
                Dest->W = q.w;

                return true;
            }

            // math::Transform → FTransform
            if (StructProp->Struct == TBaseStructure<FTransform>::Get())
            {
                auto t = std::static_pointer_cast<tsumugi::script::builtin::transform::TransformInstance>(value)->GetValue();

                FTransform* Dest = reinterpret_cast<FTransform*>(Data);
                Dest->SetLocation(FVector(t.position.x, t.position.y, t.position.z));
                Dest->SetRotation(FQuat(t.rotation.x, t.rotation.y, t.rotation.z, t.rotation.w));
                Dest->SetScale3D(FVector(t.scale.x, t.scale.y, t.scale.z));

                return true;
            }
        }
    }


    return false;
}

std::shared_ptr<tsumugi::script::object::IObject> UObjectAccessor::GetUFunction(const tstring& name) const
{
    if (!Target) return nullptr;

    UFunction* Func = Target->FindFunction(ToFName(name));
    if (!Func) return nullptr;

    // BuiltinFunctionObject にラップして返す
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

            // 戻り値（ReturnParam）の処理
            std::shared_ptr<tsumugi::script::object::IObject> ReturnValue = tsumugi::script::object::NullObject::Instance();

            for (TFieldIterator<FProperty> It(Func); It; ++It)
            {
                FProperty* Property = *It;
                if (Property->HasAnyPropertyFlags(CPF_ReturnParm))
                {
                    uint8* PropertyData = Property->ContainerPtrToValuePtr<uint8>(Buffer);
                    ReturnValue = ConvertPropertyValue(Property, PropertyData);
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
        else if (StructProperty->Struct == TBaseStructure<FRotator>::Get())
        {
            if (BuiltinInstanceValue->GetBuiltinType() == tsumugi::script::builtin::BuiltinType::Rotator)
            {
                auto Rotator = std::static_pointer_cast<tsumugi::script::builtin::rotator::RotatorInstance>(BuiltinInstanceValue);

                auto e = Rotator->GetQuaternion().ToEuler();
                *(FRotator*)PropertyData = FRotator(e.x, e.y, e.z);
                return true;
            }
        }
        else if (StructProperty->Struct == TBaseStructure<FColor>::Get())
        {
            if (BuiltinInstanceValue->GetBuiltinType() == tsumugi::script::builtin::BuiltinType::Color)
            {
                auto Color = std::static_pointer_cast<tsumugi::script::builtin::color::ColorInstance>(BuiltinInstanceValue);

                auto e = Color->GetValue();
                *(FColor*)PropertyData = FColor(e.r, e.g, e.b, e.a);
                return true;
            }
        }
        else if (StructProperty->Struct == TBaseStructure<FLinearColor>::Get())
        {
            if (BuiltinInstanceValue->GetBuiltinType() == tsumugi::script::builtin::BuiltinType::LinearColor)
            {
                auto LinearColor = std::static_pointer_cast<tsumugi::script::builtin::color::LinearColorInstance>(BuiltinInstanceValue);

                auto e = LinearColor->GetValue();
                *(FLinearColor*)PropertyData = FLinearColor(e.r, e.g, e.b, e.a);
                return true;
            }
        }
    }

    return false;
}

std::shared_ptr<script::object::IObject> UObjectAccessor::ConvertPropertyValue(class FProperty* Property, uint8* PropertyData) const
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
        else if (StructProperty->Struct == TBaseStructure<FTransform>::Get())
        {
            FTransform Value = *(FTransform*)PropertyData;
            return std::make_shared<tsumugi::script::builtin::transform::TransformInstance>();
        }
        else if (StructProperty->Struct == TBaseStructure<FRotator>::Get())
        {
            FRotator Value = *(FRotator*)PropertyData;
            return std::make_shared<tsumugi::script::builtin::rotator::RotatorInstance>(Value.Pitch, Value.Yaw, Value.Roll);
        }
        else if (StructProperty->Struct == TBaseStructure<FColor>::Get())
        {
            FColor Value = *(FColor*)PropertyData;
            return std::make_shared<tsumugi::script::builtin::color::ColorInstance>(Value.R, Value.G, Value.B, Value.A);
        }
        else if (StructProperty->Struct == TBaseStructure<FLinearColor>::Get())
        {
            FLinearColor Value = *(FLinearColor*)PropertyData;
            return std::make_shared<tsumugi::script::builtin::color::LinearColorInstance>(Value.R, Value.G, Value.B, Value.A);
        }
    }

    if (auto* ObjectProperty = CastField<FObjectProperty>(Property))
    {
        UObject* Obj = ObjectProperty->GetObjectPropertyValue(PropertyData);

        if (!Obj)
        {
            return script::object::NullObject::Instance();
        }

        return std::make_shared<UObjectAccessor>(Obj);
    }

    return tsumugi::script::object::NullObject::Instance();
    //return std::make_shared<tsumugi::script::object::ErrorObject>(tsumugi::script::object::ErrorCode::kInvalidProperty, std::unordered_map<std::string, tstring>{});
}

tstring UObjectAccessor::Inspect() const
{
    if (!Target)
        return TT("<null>");

    FString Out;

    // ヘッダ
    Out += FString::Printf(TEXT("%s (%s)\n"),
        *Target->GetName(),
        *Target->GetClass()->GetName());
    Out += TEXT("--------------------------------------------------\n");

    // =========================
    // Properties（値付き）
    // =========================
    Out += TEXT("Properties:\n");

    for (TFieldIterator<FProperty> PropIt(Target->GetClass()); PropIt; ++PropIt)
    {
        FProperty* Property = *PropIt;
        FString PropName = Property->GetName();
        FString PropType = Property->GetCPPType();
        void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Target);

        FString ValueStr = TEXT("<unsupported>");

        // Int
        if (FIntProperty* IntProperty = CastField<FIntProperty>(Property))
        {
            ValueStr = FString::FromInt(IntProperty->GetPropertyValue(ValuePtr));
        }
        // Float
        else if (FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
        {
            ValueStr = FString::SanitizeFloat(FloatProperty->GetPropertyValue(ValuePtr));
        }
        // Bool
        else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
        {
            ValueStr = BoolProperty->GetPropertyValue(ValuePtr) ? TEXT("true") : TEXT("false");
        }
        // String
        else if (FStrProperty* StrProperty = CastField<FStrProperty>(Property))
        {
            ValueStr = StrProperty->GetPropertyValue(ValuePtr);
        }
        // Name
        else if (FNameProperty* NameProperty = CastField<FNameProperty>(Property))
        {
            ValueStr = NameProperty->GetPropertyValue(ValuePtr).ToString();
        }
        // Enum
        else if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
        {
            int64 Raw = EnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(ValuePtr);
            ValueStr = EnumProperty->GetEnum()->GetNameStringByValue(Raw);
        }
        // UObject*
        else if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
        {
            UObject* Obj = ObjectProperty->GetObjectPropertyValue(ValuePtr);
            ValueStr = Obj ? Obj->GetName() : TEXT("null");
        }
        // Struct（代表的なもの）
        else if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
        {
            if (StructProperty->Struct == TBaseStructure<FVector>::Get())
            {
                FVector* Vec = StructProperty->ContainerPtrToValuePtr<FVector>(Target);
                ValueStr = Vec->ToString();
            }
            else if (StructProperty->Struct == TBaseStructure<FRotator>::Get())
            {
                FRotator* Rot = StructProperty->ContainerPtrToValuePtr<FRotator>(Target);
                ValueStr = Rot->ToString();
            }
            else if (StructProperty->Struct == TBaseStructure<FTransform>::Get())
            {
                FTransform* Tr = StructProperty->ContainerPtrToValuePtr<FTransform>(Target);
                ValueStr = Tr->ToString();
            }
            else if (StructProperty->Struct == TBaseStructure<FColor>::Get())
            {
                FColor* Col = StructProperty->ContainerPtrToValuePtr<FColor>(Target);
                ValueStr = Col->ToString();
            }
            else
            {
                ValueStr = TEXT("<struct>");
            }
        }
        // Array（要素数 + 代表値）
        else if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
        {
            FScriptArrayHelper Helper(ArrayProperty, ValuePtr);
            const int32 Num = Helper.Num();
            ValueStr = FString::Printf(TEXT("Array[%d]"), Num);

            // 代表値として先頭要素を軽く表示（必要なら拡張）
            if (Num > 0)
            {
                FProperty* Inner = ArrayProperty->Inner;
                void* ElemPtr = Helper.GetRawPtr(0);

                if (FIntProperty* IP = CastField<FIntProperty>(Inner))
                {
                    ValueStr += FString::Printf(TEXT(" = [%d, ...]"), IP->GetPropertyValue(ElemPtr));
                }
                else if (FFloatProperty* FP = CastField<FFloatProperty>(Inner))
                {
                    ValueStr += FString::Printf(TEXT(" = [%s, ...]"), *FString::SanitizeFloat(FP->GetPropertyValue(ElemPtr)));
                }
                else if (FStrProperty* SP = CastField<FStrProperty>(Inner))
                {
                    ValueStr += FString::Printf(TEXT(" = [\"%s\", ...]"), *SP->GetPropertyValue(ElemPtr));
                }
                else if (FNameProperty* NP = CastField<FNameProperty>(Inner))
                {
                    ValueStr += FString::Printf(TEXT(" = [%s, ...]"), *NP->GetPropertyValue(ElemPtr).ToString());
                }
                else if (FObjectProperty* OP = CastField<FObjectProperty>(Inner))
                {
                    UObject* Obj = OP->GetObjectPropertyValue(ElemPtr);
                    ValueStr += FString::Printf(TEXT(" = [%s, ...]"), Obj ? *Obj->GetName() : TEXT("null"));
                }
            }
        }

        Out += FString::Printf(TEXT("  %s : %s = %s\n"),
            *PropName, *PropType, *ValueStr);
    }

    Out += TEXT("\n--------------------------------------------------\n");

    // =========================
    // Methods（引数 + 戻り値）
    // =========================
    Out += TEXT("Methods:\n");

    for (TFieldIterator<UFunction> FuncIt(Target->GetClass()); FuncIt; ++FuncIt)
    {
        UFunction* Func = *FuncIt;

        // 必要なら BlueprintCallable のみ
        // if (!Func->HasAnyFunctionFlags(FUNC_BlueprintCallable))
        //     continue;

        FString FuncName = Func->GetName();

        // 引数一覧
        FString ArgList;
        for (TFieldIterator<FProperty> ParamIt(Func); ParamIt; ++ParamIt)
        {
            FProperty* Param = *ParamIt;

            if (Param->HasAnyPropertyFlags(CPF_ReturnParm))
                continue;

            FString ParamType = Param->GetCPPType();
            FString ParamName = Param->GetName();

            if (!ArgList.IsEmpty())
                ArgList += TEXT(", ");

            ArgList += FString::Printf(TEXT("%s %s"), *ParamType, *ParamName);
        }

        // 戻り値
        FString ReturnStr;
        if (FProperty* ReturnProp = Func->GetReturnProperty())
        {
            FString ReturnType = ReturnProp->GetCPPType();
            ReturnStr = FString::Printf(TEXT(" -> %s"), *ReturnType);
        }

        if (ArgList.IsEmpty())
        {
            Out += FString::Printf(TEXT("  %s()%s\n"), *FuncName, *ReturnStr);
        }
        else
        {
            Out += FString::Printf(TEXT("  %s(%s)%s\n"), *FuncName, *ArgList, *ReturnStr);
        }
    }

    return ToTString(Out);
}

} // namespace tsumugi::integration

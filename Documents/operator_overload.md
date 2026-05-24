# 演算子オーバーロード仕様（tsumugi script）

tsumugi script は、組み込みクラスおよび将来的にはユーザー定義クラスに対して  
**演算子をメソッドとして扱う**柔軟なオブジェクトモデルを採用している。

この仕組みにより、以下のようなコードが自然に動作する：

```ts
let a = Vector3(1,2,3)
let b = Vector3(4,5,6)

print(a + b)     // Vector3(5,7,9)
print(-a)        // Vector3(-1,-2,-3)
print(a == b)    // false
```

---

## 1. 基本思想：演算子はメソッド名である

tsumugi script では、  
**演算子はそのままメソッド名として扱われる。**

Evaluator は以下のように解釈する：

| 記法 | 内部的な呼び出し |
|------|------------------|
| `a + b` | `a."+"(b)` |
| `a - b` | `a."-"(b)` |
| `a * b` | `a."*"(b)` |
| `a / b` | `a."/"(b)` |
| `a == b` | `a."=="(b)` |
| `a != b` | `a."!="(b)` |
| `-a`（単項） | `a."unary-"()` |

---

## 2. 二項演算子のオーバーロード

クラスは演算子名をメソッドとして登録することで  
演算子オーバーロードを実現できる。

例：Vector3 の加算

```cpp
klass->SetInstanceMethod(TT("+"), addBuiltin);
```

ユーザーコード：

```ts
let c = a + b
```

内部的には：

```
a."+"(b)
```

が呼ばれる。

---

## 3. 単項演算子（unary operator）

単項マイナスは二項マイナスと区別するため、  
**内部メソッド名 `"unary-"` を使用する。**

```cpp
klass->SetInstanceMethod(TT("unary-"), unaryMinusBuiltin);
```

ユーザーコード：

```ts
let v = -a
```

内部的には：

```
a."unary-"()
```

また、ユーザーが直接呼べるように `"neg"` も併用できる：

```cpp
klass->SetInstanceMethod(TT("neg"), unaryMinusBuiltin);
```

---

## 4. 比較演算子

`==` / `!=` も同様にメソッドとして扱われる。

```cpp
klass->SetInstanceMethod(TT("=="), eqBuiltin);
klass->SetInstanceMethod(TT("!="), neqBuiltin);
```

ユーザーコード：

```ts
if (a == b) { ... }
```

内部的には：

```
a."=="(b)
```

が呼ばれる。

---

## 5. プリミティブ型との共存

演算子オーバーロードは **プリミティブ型の高速パスより後に評価される**。

Evaluator の順序：

1. int / float / string / array の組み込み演算  
2. `==` / `!=` のプリミティブ比較  
3. **演算子オーバーロード（本仕様）**  
4. 型不一致エラー  

これにより、既存の Monkey 互換の挙動を壊さずに  
柔軟なオーバーロードを実現している。

---

## 6. 将来的なユーザー開放について

本仕様は現在、主に組み込みクラス（Vector3 など）向けに使用されているが、  
**将来的にはユーザー定義クラスにも開放する予定**。

その場合、ユーザーは以下のように書けるようになる：

```ts
class Foo {
    func +(other) {
        return Foo(this.value + other.value)
    }

    func unary-() {
        return Foo(-this.value)
    }
}

let a = Foo(10)
let b = Foo(20)

print(a + b)   // Foo(30)
print(-a)      // Foo(-10)
```

Evaluator 側はすでに対応済みであり、  
クラス定義構文が整い次第、自然に開放できる。

---

## 7. まとめ

- 演算子はそのままメソッド名として扱われる  
- 二項演算子は `"+"`, `"-"`, `"*"`, `"/"` など  
- 単項演算子は `"unary-"`  
- 比較演算子は `"=="`, `"!="`  
- ユーザーが直接呼べる別名（例：`neg`）も併用可能  
- 将来的にユーザー定義クラスにも開放予定  

tsumugi script の演算子モデルは、  
Ruby や Smalltalk のような美しいオブジェクトモデルを持ち、  
拡張性と直感性を両立している。

## 8. Vector3 における演算子オーバーロード実装例（完全版）

以下は、tsumugi script の組み込みクラス `Vector3` に  
演算子オーバーロードを実装した実例である。

この例では：

- `+`（加算）
- `-`（減算）
- `*`（スカラー乗算）
- `/`（スカラー除算）
- `==`（等価比較）
- `!=`（非等価比較）
- `-v`（単項マイナス）

をすべて **演算子名メソッド** として登録している。

### （補足）Vector3Class の演算子メソッド登録例

```cpp
// ================================
// Vector3: 加算 (+)
// ================================
auto addBuiltin = std::make_shared<object::BuiltinFunctionObject>(
    [](std::shared_ptr<object::IObject> self,
       const std::vector<std::shared_ptr<object::IObject>>& args)
    -> std::shared_ptr<object::IObject>
    {
        auto a = std::static_pointer_cast<Vector3Instance>(self);
        auto b = std::static_pointer_cast<Vector3Instance>(args[0]);

        return std::make_shared<Vector3Instance>(
            a->X() + b->X(),
            a->Y() + b->Y(),
            a->Z() + b->Z()
        );
    }
);

// "+" と "add" の両方を登録
klass->SetInstanceMethod(TT("add"), addBuiltin);
klass->SetInstanceMethod(TT("+"), addBuiltin);


// ================================
// Vector3: 減算 (-)
// ================================
auto subBuiltin = std::make_shared<object::BuiltinFunctionObject>(
    [](std::shared_ptr<object::IObject> self,
       const std::vector<std::shared_ptr<object::IObject>>& args)
    -> std::shared_ptr<object::IObject>
    {
        auto a = std::static_pointer_cast<Vector3Instance>(self);
        auto b = std::static_pointer_cast<Vector3Instance>(args[0]);

        return std::make_shared<Vector3Instance>(
            a->X() - b->X(),
            a->Y() - b->Y(),
            a->Z() - b->Z()
        );
    }
);

klass->SetInstanceMethod(TT("sub"), subBuiltin);
klass->SetInstanceMethod(TT("-"), subBuiltin);


// ================================
// Vector3: スカラー乗算 (*)
// ================================
auto mulBuiltin = std::make_shared<object::BuiltinFunctionObject>(
    [](std::shared_ptr<object::IObject> self,
       const std::vector<std::shared_ptr<object::IObject>>& args)
    -> std::shared_ptr<object::IObject>
    {
        auto a = std::static_pointer_cast<Vector3Instance>(self);

        double s = 1.0;
        if (!args.empty()) {
            if (args[0]->GetType() == object::ObjectType::kFloat)
                s = std::static_pointer_cast<object::FloatObject>(args[0])->GetValue();
            else if (args[0]->GetType() == object::ObjectType::kInteger)
                s = static_cast<double>(std::static_pointer_cast<object::IntegerObject>(args[0])->GetValue());
        }

        return std::make_shared<Vector3Instance>(
            a->X() * s,
            a->Y() * s,
            a->Z() * s
        );
    }
);

klass->SetInstanceMethod(TT("mul"), mulBuiltin);
klass->SetInstanceMethod(TT("*"), mulBuiltin);


// ================================
// Vector3: スカラー除算 (/)
// ================================
auto divBuiltin = std::make_shared<object::BuiltinFunctionObject>(
    [](std::shared_ptr<object::IObject> self,
       const std::vector<std::shared_ptr<object::IObject>>& args)
    -> std::shared_ptr<object::IObject>
    {
        auto a = std::static_pointer_cast<Vector3Instance>(self);

        double s = 1.0;
        if (!args.empty()) {
            if (args[0]->GetType() == object::ObjectType::kFloat)
                s = std::static_pointer_cast<object::FloatObject>(args[0])->GetValue();
            else if (args[0]->GetType() == object::ObjectType::kInteger)
                s = static_cast<double>(std::static_pointer_cast<object::IntegerObject>(args[0])->GetValue());
        }

        if (s == 0.0) {
            // 0除算は 0 ベクトルを返す（仕様は必要に応じて変更可能）
            return std::make_shared<Vector3Instance>(0, 0, 0);
        }

        return std::make_shared<Vector3Instance>(
            a->X() / s,
            a->Y() / s,
            a->Z() / s
        );
    }
);

klass->SetInstanceMethod(TT("div"), divBuiltin);
klass->SetInstanceMethod(TT("/"), divBuiltin);


// ================================
// Vector3: 等価比較 (==)
// ================================
auto eqBuiltin = std::make_shared<object::BuiltinFunctionObject>(
    [](std::shared_ptr<object::IObject> self,
       const std::vector<std::shared_ptr<object::IObject>>& args)
    -> std::shared_ptr<object::IObject>
    {
        auto a = std::static_pointer_cast<Vector3Instance>(self);
        auto b = std::static_pointer_cast<Vector3Instance>(args[0]);

        bool same =
            a->X() == b->X() &&
            a->Y() == b->Y() &&
            a->Z() == b->Z();

        return object::BooleanObject::FromBool(same);
    }
);

klass->SetInstanceMethod(TT("eq"), eqBuiltin);
klass->SetInstanceMethod(TT("=="), eqBuiltin);


// ================================
// Vector3: 非等価比較 (!=)
// ================================
auto neqBuiltin = std::make_shared<object::BuiltinFunctionObject>(
    [](std::shared_ptr<object::IObject> self,
       const std::vector<std::shared_ptr<object::IObject>>& args)
    -> std::shared_ptr<object::IObject>
    {
        auto a = std::static_pointer_cast<Vector3Instance>(self);
        auto b = std::static_pointer_cast<Vector3Instance>(args[0]);

        bool same =
            a->X() == b->X() &&
            a->Y() == b->Y() &&
            a->Z() == b->Z();

        return object::BooleanObject::FromBool(!same);
    }
);

klass->SetInstanceMethod(TT("neq"), neqBuiltin);
klass->SetInstanceMethod(TT("!="), neqBuiltin);


// ================================
// Vector3: 単項マイナス (-v)
// ================================
auto unaryMinusBuiltin = std::make_shared<object::BuiltinFunctionObject>(
    [](std::shared_ptr<object::IObject> self,
       const std::vector<std::shared_ptr<object::IObject>>&)
    -> std::shared_ptr<object::IObject>
    {
        auto a = std::static_pointer_cast<Vector3Instance>(self);

        return std::make_shared<Vector3Instance>(
            -a->X(),
            -a->Y(),
            -a->Z()
        );
    }
);

// "unary-" は Evaluator が使う内部用
// "neg" はユーザーが直接呼べる API
klass->SetInstanceMethod(TT("unary-"), unaryMinusBuiltin);
klass->SetInstanceMethod(TT("neg"), unaryMinusBuiltin);
```

---

### この実装のポイントまとめ

- **演算子名そのものをメソッド名として登録**する  
  → Evaluator が `a + b` を `a."+"(b)` に変換して呼び出す

- **ユーザー向けの別名（add, sub, mul, div, neg）も併用可能**  
  → API としても自然で読みやすい

- **単項マイナスは "unary-" を使うことで二項マイナスと衝突しない**

- **比較演算子も同じ仕組みで実装できる**

この例は、tsumugi script の演算子オーバーロードの  
“最も美しい実装形” として参考にできる。

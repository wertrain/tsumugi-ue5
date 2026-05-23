# Tsumugi Script – オブジェクトメソッド統一化リファクタリング案

この文書は、Tsumugi Script のオブジェクトモデルにおける  
**「TryGetProperty の責務統一」** と  
**「メソッドテーブル方式への移行」** を目的とした設計メモです。

現状の課題と改善案を整理し、将来のリファクタリング指針として残します。

---

## 現状の課題

### 1. 型ごとに TryGetProperty の責務が異なる

- **Array / Hash / String**  
  → TryGetProperty が「プロパティ + メソッドディスパッチ」を担当  
  → GetArrayProperty / GetHashProperty などに委譲

- **UserObject**  
  → TryGetProperty が「プロパティ探索 + prototype チェーン」のみ  
  → メソッドを持たない（has を入れると例外的になる）

この差により、**UserObject だけ仲間外れ**になっている。

---

### 2. ArrayObject のメソッド実装が巨大 if 文

- push / pop / slice / join がすべて if 文で直書きされている  
- メソッド追加時に TryGetProperty が肥大化する  
- 他の型との構造差が大きい

---

### 3. メソッドの初期化が分散している

- ArrayMethods.h はあるが、実際には巨大 if 文の中に実装がある  
- HashMethods / StringMethods も同様  
- メソッドテーブル方式に統一されていない

---

## 目標

- TryGetProperty の責務を全型で統一する  
- メソッドは「メソッドテーブル」で一元管理する  
- UserObject もメソッドを持てるようにする（has など）  
- 巨大 if 文を排除し、拡張性を高める  
- 言語仕様として美しい構造にする  

---

## 提案：メソッドテーブル方式への統一

### 1. 各オブジェクト型にメソッドテーブルを持たせる

例：ArrayMethods

```cpp
struct ArrayMethods {
    static std::unordered_map<tstring, std::shared_ptr<IObject>> methods;
    static void Init();
};
```

HashMethods / StringMethods / UserMethods も同様。

---

### 2. TryGetProperty の統一構造

全型で以下の順番に統一する：

1. 自分のプロパティを見る（UserObjectのみ）  
2. メソッドテーブルを見る（全型）  
3. prototype チェーンを見る（UserObjectのみ）  
4. なければ std::nullopt  

ArrayObject の例：

```cpp
std::optional<std::shared_ptr<IObject>> ArrayObject::TryGetProperty(const tstring& name) {

    if (name == TT("length")) {
        return std::make_shared<IntegerObject>(elements_.size());
    }

    auto it = ArrayMethods::methods.find(name);
    if (it != ArrayMethods::methods.end()) {
        return it->second;
    }

    return std::nullopt;
}
```

---

### 3. メソッド初期化（Init）の呼び出し場所

- ScriptEngine::Init()  
- Evaluator / VM の初期化フェーズ  
などで **一度だけ**呼ぶ。

例：

```cpp
void ScriptEngine::Init() {
    ArrayMethods::Init();
    HashMethods::Init();
    StringMethods::Init();
    UserMethods::Init(); // has() など
}
```

メソッドテーブルは static なので、Init は一度だけ呼ぶ。

---

## メリット

- TryGetProperty が全型で統一される  
- 巨大 if 文が消える  
- メソッド追加が 1 行で済む  
- UserObject も自然にメソッドを持てる  
- 言語仕様として美しい  
- 拡張性が高い（Map, Set, Date など追加しやすい）  

---

## デメリット（軽微）

- 初期化コード（Init）が必要  
- 既存コードの移行コストがある  
- メソッドテーブルの static 管理が必要  

---

## 結論

このリファクタリングは今すぐやらなくても良いが、  
将来の拡張性と一貫性を大幅に改善する。

特に：

- UserObject の仲間外れ問題  
- ArrayObject の巨大 if 文  
- メソッド追加の煩雑さ  

これらを根本的に解決できる。

Tsumugi Script のオブジェクトモデルを美しく保つための  
**重要な改善案として、将来の候補に残す価値が高い。**

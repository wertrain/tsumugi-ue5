# Builtin Type System Design  
**（tsumugi Script Runtime – Builtin 型システム設計メモ）**

## 概要
tsumugi の Builtin クラス（Vector2 / Vector3 / Quaternion / Random など）の  
型判定方式をどう設計するかについてのメモ。

最終的に採用した方式は **enum による型識別**だが、  
以前検討していた **StaticClassName（tstring_view）方式**にも利点があるため、  
両者の比較と採用理由をここに記録する。

---

## 1. StaticClassName（tstring_view）方式

### 仕組み

    #define DEFINE_BUILTIN_CLASS(name) \
        static inline constexpr tstring_view StaticClassName = TT(name); \
        tstring_view GetClassName() const override { return StaticClassName; }

型判定は文字列比較で行う：

    if (inst->GetClassName() == QuaternionInstance::StaticClassName)

### メリット

圧倒的な強み：
新しい組み込みクラス（例: Matrix）を作るとき、そのヘッダー1箇所に DEFINE_BUILTIN_CLASS("Matrix") と書くだけで世界が完結する。 中央の管理ファイル（enum定義など）を書き換えに行く必要が一切ないため、コードのトポロジーが非常にクリーンです。

- 実装が簡単で直感的  
- クラス名がそのままコードに現れる  
- マクロで簡単に導入できる  
- 小規模なスクリプトランタイムでは十分実用的  

### デメリット
- 文字列比較は遅い（ハッシュしても enum より遅い）  
- クラス名 typo がコンパイル時に検出されない  
- Instance クラスを include しないと名前が取れない  
- 循環依存が起きやすい  
- クラス名の一元管理ができない  

### 評価
小規模・軽量なランタイムなら十分に成立する方式。  
ただし、tsumugi のように Builtin クラスが増え、  
型判定が頻繁に行われる環境ではスケールしにくい。

---

## 2. enum + 名前テーブル方式（採用）

### 仕組み

    enum class BuiltinType {
        Vector2,
        Vector3,
        Quaternion,
        Random,
        // ...
    };

Instance は enum を保持：

    class QuaternionInstance : public BuiltinInstanceObject {
    public:
        QuaternionInstance() : BuiltinInstanceObject(BuiltinType::Quaternion) {}
    };

型判定：

    if (inst->GetBuiltinType() == BuiltinType::Quaternion)

### メリット
- 高速（整数比較）  
- 型名の一元管理が可能  
- Instance の include が不要  
- 循環依存が起きない  
- typo がコンパイル時に検出される  
- switch-case が使える（高速）  
- AST / Object の既存設計と統一できる  

### デメリット
- 初期実装の手間が少し増える  
- 名前テーブルを別途管理する必要がある  

### 評価
言語処理系・VM として長期的に拡張するなら最適。  
tsumugi の AST / Object が enum 方式を採用しているため、  
ランタイムも同方式に統一することで設計が一貫する。

---

## 3. 結論

### ✔ 採用：enum + 名前テーブル方式  
理由：
- 高速  
- 拡張性が高い  
- AST / Object と統一できる  
- 循環依存を避けられる  
- 型判定が安全（コンパイル時チェック）

### ✔ StaticClassName 方式も「悪いわけではない」  
- 小規模ランタイムでは十分  
- 実装が簡単で理解しやすい  
- プロトタイプベースのオブジェクトモデルと相性が良い  
- 文字列ベースの柔軟性がある

### ✔ しかし tsumugi の規模・方向性を考えると enum が最適  
- Builtin クラスが増える  
- Transform / GameObject / Component なども増える  
- 型判定が頻繁に発生する  
- パフォーマンスと安全性が重要

---

## 4. 今後の方針

- BuiltinType enum を中心に据えた型システムを構築する  
- クラス名は BuiltinTypeName() テーブルで管理  
- StaticClassName 方式は廃止ではなく「参考方式」として残す  
  - 必要ならプラグイン側で使える  
  - デバッグ用途にも便利  

---

## 付録：名前テーブル例

    inline const tchar* BuiltinTypeName(BuiltinType t) {
        switch (t) {
        case BuiltinType::Vector2: return TT("Vector2");
        case BuiltinType::Vector3: return TT("Vector3");
        case BuiltinType::Quaternion: return TT("Quaternion");
        case BuiltinType::Random: return TT("Random");
        }
        return TT("Unknown");
    }


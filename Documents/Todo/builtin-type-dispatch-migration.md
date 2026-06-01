# tsumugi 組み込みクラスを「型ディスパッチ方式」に移行するためのロードマップ

この文書は、tsumugi の組み込みクラス（Vector3 / Quaternion / Matrix4x4 / Transform など）を  
従来の「プロトタイプ方式（プロパティ辞書ベース）」から  
「型ディスパッチ方式（C++ の math 型を直接保持する方式）」へ移行するための  
背景・目的・メリット・具体的な TODO をまとめたものです。

---

# 1. なぜ型ディスパッチ方式に移行するのか（経緯と背景）

tsumugi の組み込みクラスは、初期は **JavaScript 的なプロトタイプ方式**で実装されていました。

- プロパティ辞書に値を格納する  
- `TrySetProperty` で動的に型チェック  
- `Inspect()` は文字列を組み立てるだけ  
- Vector3 / Quaternion / Transform も「ただの辞書」

この方式は **実装が簡単で柔軟**というメリットがある一方で、  
次のような問題が明確になってきました。

### ❌ パフォーマンスが低い
辞書アクセスは遅く、Vector3 のような頻繁に使う型ではボトルネックになる。

### ❌ 型安全性が弱い
Vector3 + Quaternion のような不正な演算が実行時まで検出できない。

### ❌ Transform のような複雑な型が扱いにくい
position / rotation / scale を辞書で持つと、  
内部値とプロパティの同期が煩雑でバグの温床になる。

### ❌ C++ 側の math ライブラリを活かせない
せっかく高性能な math::Vector3 / Quaternion / Matrix4x4 があるのに、  
辞書ベースでは直接使えない。

---

# 2. 型ディスパッチ方式のメリット

型ディスパッチ方式では、  
**組み込みクラスが C++ の math 型を内部に直接保持する**。

例：

    class Vector3Instance : public BuiltinInstanceObject {
        math::Vector3 value_;
    };

これにより、次のメリットが得られる。

### ✔ 圧倒的な高速化
辞書アクセス → 直接メンバアクセス  
演算も C++ の math 型をそのまま使える。

### ✔ 型安全
不正な演算をコンパイル時に防げる。

### ✔ Transform が本物になる
forward / up / right / toMatrix が高速で正確に動く。

### ✔ ゲームエンジンとの連携が容易
C++ 側の Transform / Vector3 と 1:1 で対応できる。

### ✔ プロパティ辞書を廃止できる
動的型の弱点が消え、  
tsumugi が「強い型を持つスクリプト言語」に進化する。

---

# 3. 型ディスパッチ方式への移行 TODO（完全版）

## 3.1 BuiltinInstanceObject の整理
- [ ] プロパティ辞書（Set/Get/TrySetProperty）を段階的に廃止
- [ ] Inspect() を純粋仮想にし、各型が独自実装するようにする
- [ ] GetType() を BuiltinType による型判定に統一
- [ ] C++ 側の内部値を保持するための標準的な仕組みを導入  
  - 例: `Vector3Instance { math::Vector3 value_; }`

---

## 3.2 BuiltinClassObject の Instantiate を型ディスパッチ対応にする
- [ ] Instantiate<T>(args...) を正式 API として確立
- [ ] インスタンス生成時に prototype を設定
- [ ] コンストラクタ引数チェックを追加
- [ ] optional 返却で失敗を安全に扱う
- [ ] プロパティ辞書を使わず、C++ の内部値を直接初期化する

---

## 3.3 Vector3 / Quaternion / Matrix4x4 / Transform の Instance クラスを型ディスパッチ化

### Vector3Instance
- [ ] math::Vector3 value_ を内部に持つ
- [ ] GetValue() / SetValue() を追加
- [ ] Inspect() を math::Vector3 から生成
- [ ] プロパティ辞書を削除
- [ ] x, y, z を getter/setter メソッドにする

### QuaternionInstance
- [ ] math::Quaternion value_ を内部に持つ
- [ ] Rotate, Normalized, Slerp などをメソッド化
- [ ] Inspect() を math::Quaternion から生成

### Matrix4x4Instance
- [ ] math::Matrix4x4 value_ を内部に持つ
- [ ] MultiplyPoint, MultiplyVector をメソッド化
- [ ] Inspect() を行列形式で出力

### TransformInstance
- [ ] position_, rotation_, scale_ を math 型で保持
- [ ] Forward(), Up(), Right() を math ベースで実装
- [ ] ToMatrix() を math::Matrix4x4 で返す
- [ ] プロパティ辞書を完全削除
- [ ] TrySetProperty を廃止し setter メソッドに統一

---

## 3.4 BuiltinClassRegistry の責務を“辞書”に限定
- [ ] CreateInstance<T>() を削除
- [ ] クラスの登録・取得だけに責務を限定
- [ ] インスタンス生成は BuiltinClassObject に一本化

---

## 3.5 Evaluator の型ディスパッチ対応
- [ ] 二項演算（+ - * /）を型ごとにディスパッチ  
  - Vector3 + Vector3  
  - Quaternion * Vector3  
  - Matrix4x4 * Vector3  
- [ ] 比較演算（== !=）を型ごとに実装
- [ ] 関数呼び出し時に BuiltinFunctionObject を優先
- [ ] クラスメソッド呼び出しを型安全にする

---

## 3.6 プロパティアクセスの仕様変更
- [ ] `obj.x` → getter メソッド呼び出しに変換
- [ ] `obj.x = 10` → setter メソッド呼び出しに変換
- [ ] プロパティ辞書を完全に廃止
- [ ] AST の PropertyExpression をメソッド呼び出しに変換する

---

## 3.7 エラーメッセージの統一
- [ ] 型エラーを TypeMismatchError に統一
- [ ] メソッド未定義を NoSuchMethodError に統一
- [ ] インスタンス生成失敗を ConstructorError に統一
- [ ] ErrorReporter をグローバル 1 個に統一

---

## 3.8 テストケースの更新
- [ ] Vector3 の演算テスト
- [ ] Quaternion の回転テスト
- [ ] Matrix4x4 の変換テスト
- [ ] Transform の forward/up/right テスト
- [ ] プロパティ辞書廃止後の動作確認
- [ ] import / module 対応後のテスト（次フェーズ）

---

## 3.9 移行フェーズの段階的対応
- [ ] プロパティ辞書と型ディスパッチを併存させる“移行モード”を作る
- [ ] 全クラスが型ディスパッチ化されたらプロパティ辞書を削除
- [ ] 旧 API（Set/Get）を非推奨にする
- [ ] 最終的に削除

---

# 4. この移行が終わると tsumugi はどう変わる？

### ✔ Transform が本物のゲームエンジンレベルになる  
Unity / Unreal と同じように、  
position / rotation / scale が math 型で高速に扱える。

### ✔ Vector3 / Quaternion / Matrix4x4 が高速で安全  
C++ の math ライブラリをそのまま使える。

### ✔ スクリプトが“型を持つ言語”になる  
プロパティ辞書の弱点が消え、  
型安全で高速なスクリプト言語になる。

### ✔ import / module system の基盤が整う  
型ディスパッチ方式はモジュール化と相性が良い。

---

# 5. 推奨する実装順序

1. BuiltinInstanceObject の整理  
2. Vector3Instance の型ディスパッチ化（最も簡単）  
3. Quaternion → Matrix4x4 → Transform の順で進める  
4. Evaluator の型ディスパッチ対応  
5. プロパティ辞書の完全廃止  

---

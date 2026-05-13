# tsumugi: プロパティアクセス・関数呼び出しモデルと Monkey 的オブジェクト設計思想

## 1. 概要

tsumugi のプロパティアクセスおよび関数呼び出しモデルは、
Monkey 言語の思想をベースにしつつ、将来的な UObject（ユーザー定義オブジェクト）や
メソッドシステムの拡張を見越した設計になっている。

本ドキュメントでは以下をまとめる：

- 現在のプロパティアクセスの仕組み
- 関数呼び出しモデル（InvokeFunction）
- Monkey 的「ラッピングによる拡張」思想
- BoundMethod の設計
- tsumugi の現在の設計との差分
- 将来のメソッドシステムへの拡張性

---

## 2. 現在のプロパティアクセスの仕組み

### 2.1 AST レベル

プロパティアクセスは以下の 2 種類の AST ノードで表現される：

- PropertyAccessExpression（obj.x）
- IndexExpression（obj["x"]）

### 2.2 Evaluator の役割

Evaluator は AST の評価に専念し、
プロパティアクセスの実体は ObjectProtocolDispatcher に委譲される。

### 2.3 ObjectProtocolDispatcher の役割

Dispatcher は「オブジェクトが持つプロトコル」を統一的に扱うレイヤー。

- TryGetProperty
- （将来）TrySetProperty
- （将来）TryInvokeMethod

など、型ごとに異なるプロパティの扱いを Evaluator に書かずに済む。

---

## 3. 関数呼び出しモデル（InvokeFunction）

tsumugi の関数呼び出しは InvokeFunction に集約されている。

InvokeFunction の責務：

- 関数呼び出しの統一的な入口
- クロージャ環境の構築
- self（レシーバ）の注入（receiver がある場合）
- return の unwrap（Monkey 互換のための副次的処理）

Evaluator は AST の評価に専念し、
関数呼び出しの詳細は InvokeFunction が担当する。

---

## 4. Monkey 的オブジェクト設計思想
（“wrap, don’t reinvent”）

Monkey の設計思想の肝は：

> 新しい巨大な概念を追加するのではなく、
> 既存のオブジェクトをラップして新しい振る舞いを与えること

にある。

- 関数 = コード本体 + 定義時環境
- メソッド = 関数 + 実行時ターゲット（receiver）

Monkey は「Function を拡張する」のではなく、
「Function をラップする」ことでメソッドを表現する。

この思想の延長線上にあるのが BoundMethod。

---

## 5. BoundMethod という発想

### 5.1 BoundMethod が解決する問題

通常の関数：

    Function = { Parameters, Body, Env }

メソッドに必要なもの：

    Receiver（実行対象のインスタンス）

Monkey 的解決策：

    Function 自体はそのままにしておき、
    「Function + Receiver」をまとめた BoundMethod を導入する。

### 5.2 データ構造（Go の例）

    type BoundMethod struct {
        Receiver Object
        Function *object.Function
    }

    func (bm *BoundMethod) Type() object.ObjectType { return BOUND_METHOD_OBJ }

    func (bm *BoundMethod) Inspect() string {
        return fmt.Sprintf("method: %s", bm.Function.Inspect())
    }

tsumugi なら：

- UserFunctionObject はそのまま
- BoundMethodObject（仮）を追加し、receiver + function を保持する

という形になる。

---

## 6. BoundMethod の評価フロー

### 6.1 プロパティアクセス時にラップする

`taro.sayHi` を評価すると：

1. `taro` を評価
2. `sayHi` を探す
3. 関数を返すのではなく BoundMethod(receiver=taro, function=sayHi) を返す

### 6.2 Call 時の処理

`taro.sayHi()` を評価すると：

1. callable が BoundMethod か判定
2. BoundMethod なら：
   - Receiver と Function を取り出す
   - 新しい Environment を作る
   - self（または this）を束縛
   - その環境で Function.Body を実行

これにより：

- クロージャ（定義時環境）は壊れない
- self（実行時コンテキスト）だけが追加される

---

## 7. BoundMethod が優れている理由

### 7.1 Function 型を汚さない

- Function に receiver フィールドを追加しない
- メソッドであることは「利用時の文脈」に閉じ込められる

### 7.2 クロージャとの親和性

- Function は定義時の Environment を保持
- BoundMethod は実行時の receiver を保持

役割が綺麗に分離される。

### 7.3 構文が自然

    let f = taro.sayHi
    f()  // それでも taro のコンテキストで実行される

BoundMethod 方式ならこれが自然に実現できる。

---

## 8. “よりピュアな Monkey” を目指すなら

### 8.1 self を Environment に注入する方式（tsumugi 向き）

- 新しい Environment を作る
- self を束縛
- その環境で Function を実行

Monkey の Environment モデルと相性が良い。

### 8.2 暗黙の第一引数方式（より関数的）

- メソッド呼び出し時に receiver を第一引数として渡す
- Python の self に近い

ただし引数リストがズレるため、tsumugi では不自然。

---

## 9. tsumugi の現在の設計との差分（重要）

### 9.1 tsumugi の現在の設計

tsumugi は現在：

- UserFunctionObject = { parameters, body, environment }
- InvokeFunction が関数呼び出しの中心
- ObjectProtocolDispatcher がプロパティアクセスを担当
- プロパティアクセスは「値を返すだけ」
- self の注入は「InvokeFunction の引数として receiver が渡された場合のみ」

つまり：

> 関数呼び出しモデルは完成しているが、  
> メソッド呼び出しモデルはまだ存在しない。

### 9.2 BoundMethod との違い

Monkey 的 BoundMethod：

- プロパティアクセス時に「関数 → BoundMethod」に変換
- BoundMethod が receiver を保持
- Call 時に self を注入

tsumugi の現状：

- プロパティアクセスは「ただの値取得」
- 関数を返すだけで receiver 情報は付与されない
- self の注入は「InvokeFunction に receiver が渡された場合のみ」

つまり：

> tsumugi は「関数呼び出しモデルは Monkey と同等に強いが、  
> メソッド呼び出しモデルはまだ未実装」。

### 9.3 BoundMethod を導入するとどう変わるか

- obj.method の評価結果が BoundMethod になる
- BoundMethod が receiver を保持
- InvokeFunction が BoundMethod を解釈して self を注入
- `let f = obj.method; f();` が自然に動く

つまり：

> 現在の tsumugi の設計に、  
> メソッド呼び出しの自然な拡張がそのまま乗る。

### 9.4 tsumugi の設計が BoundMethod と相性が良い理由

- InvokeFunction が「関数呼び出しの唯一の窓口」
- Dispatcher がプロパティアクセスを一元管理
- UserFunctionObject がクロージャ環境を保持

これらはすべて：

> BoundMethod を追加するだけでメソッドシステムが完成する  
> という理想的な前提条件になっている。

---

## 10. まとめ

- Monkey の思想は「既存のオブジェクトをラップして振る舞いを拡張する」
- メソッドは「Function + Receiver」をまとめた BoundMethod で表現できる
- BoundMethod は Function を汚さず、クロージャと相性が良い

# tsumugi: プロパティアクセスと関数呼び出しモデルの設計経緯

## 1. 概要

tsumugi のプロパティアクセスおよび関数呼び出しモデルは、
Monkey 言語の思想をベースにしつつ、将来的な UObject（ユーザー定義オブジェクト）や
メソッドシステムの拡張を見越した設計になっている。

本ドキュメントでは以下をまとめる：

- 現在のプロパティアクセスの仕組み
- なぜこの設計に至ったか（Monkey との違い）
- 完全にピュアな Monkey ならどう設計すべきだったか
- 今後の拡張性

---

## 2. 現在のプロパティアクセスの仕組み

### 2.1 AST レベル

プロパティアクセスは次の 2 種類の AST ノードで表現される：

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

これにより、型ごとに異なるプロパティの扱いを Evaluator に書かずに済む。

対象となるオブジェクト例：

- HashObject
- ArrayObject
- StringObject
- UserObject（将来）

---

## 3. 関数呼び出しモデルの整理

今回の大きな改善点は InvokeFunction の導入。

### 3.1 InvokeFunction の責務

- 関数呼び出しの統一的な入口
- クロージャ環境の構築
- self（レシーバ）の注入
- return の unwrap（Monkey 互換のため）

Evaluator は AST の評価に専念し、
関数呼び出しの詳細は InvokeFunction に集約される。

---

## 4. なぜこの設計にしたのか（経緯）

### 4.1 当初の問題点

以前の実装では：

- ObjectProtocolDispatcher が evaluator を持っていた
- 関数呼び出し（Invoke）が Dispatcher 側にあった

### 4.2 原因

- Invoke が Dispatcher に存在したため、責務が混在していた

### 4.3 解決策

Monkey の思想に従いつつ、将来の UObject やメソッドシステムを見越して：

- InvokeFunction を Evaluator に戻す
- ReturnValue の unwrap を InvokeFunction に実装する
- Dispatcher はプロパティアクセス専用にする

という構造に整理した。

---

## 5. 完全にピュアな Monkey ならどう設計すべきだったか

Monkey の純粋な仕様に従うなら：

- プロパティアクセスは存在しない
- ObjectProtocolDispatcher も存在しない
- 関数呼び出しは ApplyFunction のみ
- self（レシーバ）という概念もない
- メソッド呼び出しも存在しない

つまり、プロパティアクセスやメソッド呼び出しを前提とした設計は Monkey にはない。

ピュア Monkey の Evaluator は：

- AST を直接評価
- 関数呼び出しは ApplyFunction のみ
- オブジェクトのプロトコルは存在しない
- すべての型はプリミティブ的に扱われる

という非常にシンプルな構造になる。

---

## 6. tsumugi の設計が優れている点

tsumugi の設計は Monkey よりも拡張性が高い。

将来的に追加可能な機能：

- プロパティアクセス
- メソッド呼び出し
- UObject（ユーザー定義オブジェクト）
- クラスシステム
- プロトコル（インターフェース）
- メタプログラミング

これらを自然に支えられる構造になっている。

特に：

- Evaluator → AST の評価
- Dispatcher → オブジェクトの振る舞い
- InvokeFunction → 関数呼び出しの統一窓口

という三層構造は、言語処理系として非常に美しい。

---

## 7. まとめ

今回の設計変更で得られたもの：

- Evaluator と Dispatcher の責務分離
- InvokeFunction による関数呼び出しモデルの統一
- ReturnValue の正しい伝播と unwrap
- プロパティアクセスの統一的な扱い
- 将来の UObject / メソッドシステムへの自然な拡張性

Monkey の思想を尊重しつつ、
より現代的で拡張可能な言語基盤に進化した。

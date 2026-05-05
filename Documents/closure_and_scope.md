# クロージャとスコープ管理の設計メモ

本ドキュメントは tsumugi Script（Monkey 派生）の
Environment / FunctionObject / Evaluator の設計意図をまとめたものです。

C++ では GC が存在しないため、shared_ptr / weak_ptr を用いた
手動的な循環参照管理が必要になる。
本メモはその背景と理由を明確にするための技術文書である。

---

## 1. Environment の設計

### ● store_（変数束縛）
```cpp
std::unordered_map<tstring, std::shared_ptr<IObject>> store_;
```
変数名 → 値（IObject）を保持する。

shared_ptr を使うのは、値オブジェクトの所有権を Environment が持つため。

### ● outer_（外側スコープ）
```cpp
std::weak_ptr<Environment> outer_;
```
親スコープを参照するが、所有権は持たない。

weak_ptr を使う理由は Environment 同士の循環参照を防ぐため。

## 2. FunctionObject の設計

### ● environment_（クロージャ環境）
```cpp
std::shared_ptr<Environment> environment_;
```
なぜ shared_ptr なのか？
クロージャは「定義時の環境」を延命する必要があるため。

weak_ptr にすると、外側スコープが破棄された瞬間にクロージャが壊れる。

```
例
monkey
let f = function(x) {
    function(y) { x + y }
};
let g = f(2);
g(3); // x=2 を保持している必要がある
```

## 3. 循環参照が発生する理由

以下の構造が成立する：

```
Environment.store_ → FunctionObject（shared_ptr）
FunctionObject.environment_ → Environment（shared_ptr）
```
これは 2 ノードの循環参照 であり、
C++ の shared_ptr は循環を解決できないため メモリリーク となる。

## 4. Clear() の役割

Environment::Clear() は store_ を破棄し、
Environment → FunctionObject の参照を切る。

これにより循環参照が解消され、
FunctionObject.environment_ も解放される。

### ● Clear を呼ばない場合

循環参照が残り、_CrtDumpMemoryLeaks() でリークとして報告される。

これは 設計上の仕様 であり、バグではない。

## 5. env_stack_ 方式（代替案）

Evaluator 側でスコープをスタック管理する方式。

### ● 概要
関数呼び出し時に新しい Environment を push

終了時に pop

* if / while / block も push/pop で管理
* Environment.outer_ は weak_ptr のため、Environment 同士の循環は発生しない
* FunctionObject.environment_ は shared_ptr のまま（クロージャ保持）

### ● メリット

#### Clear() を呼ばなくても循環参照が自然に解消される

スコープ階層が Evaluator によって明示的に管理される

### ● デメリット
Evaluator がステートフルになる

push/pop の漏れが致命的なバグになる

#### 実装が複雑化する

Eval の引数 env と env_stack_.back() の二重構造が混乱を招く

### ● 結論
本プロジェクトでは Evaluator をステートレスに保つため、
env_stack_ 方式は採用せず、Environment::Clear() を明示的に呼ぶ方式を採用する。

## 6. 最終まとめ

* FunctionObject.environment_ は shared_ptr（クロージャ保持のため）
* Environment.outer_ は weak_ptr（循環防止のため）
* Environment.store_ は shared_ptr（変数束縛のため）
* この構造は必ず循環参照を生むため、Environment::Clear() が必要
* Clear を呼ばない場合、リークとして検出されるが仕様である
* env_stack_ 方式は循環を自然解消できるが、Evaluator がステートフルになるため採用しない

以上が本プロジェクトのスコープ管理・クロージャ設計の全体像である。
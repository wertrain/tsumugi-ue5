# Tsumugi Script – super.method のエラー処理（未実装タスク）

この文書は、Tsumugi Script における  
`super.method()` 呼び出しの **未実装部分（エラー処理）** を  
将来のリファクタリングタスクとして記録するもの。

---

## 🔍 現状の問題

以下のコードを実行すると：

```ts
class A { speak() { "A" } }
class B extends A { shout() { super.shout() } }
B().shout()
```

現在の結果：

```
Invalid null.
```

これは内部的にこうなっている：

1. `super.shout`  
   → 親クラス A に `shout` が存在しない  
   → `null` が返る

2. `null()` を呼ぼうとして例外  
   → `Invalid null.` という不適切なエラーになる

---

## 🎯 本来あるべき挙動

`super.shout()` を呼んだ時点で：

- 親クラスに `shout` が存在しない  
- よって **プロパティが存在しないエラー** を返すべき

例：

```
Invalid property: shout
```

---

## 🧩 未実装のポイント（やるべきこと）

### ❗ super.method の存在チェックを Invoke に追加する

現在の Invoke（関数呼び出し処理）は：

- `receiver.property` を取得  
- それが null でも関数呼び出しを試みる  
- 結果として null() → Invalid null

これを修正し、以下のロジックを追加する：

```
if (super lookup returns null)
    → "Invalid property: <name>" を返す
```

つまり：

- `super.method` の段階で null を許さない  
- 呼び出し前に「存在しないメソッド」を検出する

---

## 🛠 実装方針（概要）

1. `super.method` の解決処理で  
   - 親クラスの prototype を探索  
   - 見つからなければ **エラーオブジェクトを返す**

2. Invoke 側で  
   - エラーオブジェクトなら即 return  
   - null の関数呼び出しを防ぐ

---

## 📌 まとめ

super.method の完全実装に必要なのは **あと 1 点だけ**：

- 親に存在しないメソッドを super で呼んだときのエラー処理

これを実装すれば：

- super の挙動が正しくなる  
- null 呼び出しによる不適切なエラーが消える  
- 言語仕様として自然な動作になる

将来のリファクタリング候補として、このタスクを残しておく。

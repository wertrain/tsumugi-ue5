# Tsumugi Script – undefined を導入しない方針（仕様メモ）

この文書は、Tsumugi Script における  
**undefined の導入を行わない** という設計判断を記録するためのメモ。

将来の仕様変更やリファクタリング時に参照できるよう、  
理由・背景・代替手段を整理する。

---

## 🎯 結論

Tsumugi Script では **undefined を導入しない**。

代わりに：

- **未定義プロパティアクセスは null を返す**
- **プロパティ存在チェックは `obj.has(key)` で行う**

という仕様に統一する。

---

## 🔍 背景

JavaScript には `null` と `undefined` の 2 種類の「空値」が存在するが、  
これは以下のような複雑さを生む：

- truthiness が複雑になる  
- deep equality の仕様が難しくなる  
- 「意図的な null」と「未定義の undefined」の区別がバグの温床  
- 言語仕様が重くなる  
- 実装コストが増える

Tsumugi Script は：

- truthiness がシンプル  
- deep equality がある  
- prototype ベースの UserObject がある  
- null が既に存在する

これらの理由から、**undefined を導入すると JS の複雑さを輸入するだけ**になる。

---

## 🧩 仕様方針

### 1. 未定義プロパティアクセスは null を返す

例：

```ts
let a = { x: 1 }
a.y      // null
```

### 2. プロパティ存在チェックは has(key) を使う

```ts
a.has("x")  // true
a.has("y")  // false
```

### 3. has(key) は UserObject のメソッドとして提供する

- Array / Hash / String には has を追加しない  
- UserObject のみが「動的プロパティ」を持つため

### 4. undefined は言語仕様に存在しない

- 値としても型としても導入しない  
- truthiness に undefined を追加しない  
- deep equality に undefined を追加しない  
- エラー処理にも undefined を使わない

---

## 🧠 この方針のメリット

### ✔ 言語仕様がシンプルになる
null だけで「値がない」を表現できる。

### ✔ deep equality が壊れない
undefined を追加すると比較ルールが複雑化する。

### ✔ truthiness がシンプルなまま
false / null / 0 / "" のみで十分。

### ✔ 実装が軽い
undefined の型・生成・比較・表示・エラー処理が不要。

### ✔ ユーザーが混乱しない
JS の「二種類の空値問題」を避けられる。

---

## 🧩 代替手段

### 1. has(key) による存在チェック

```ts
obj.has("prop")
```

### 2. null による「値がない」の表現

```ts
return null
```

### 3. エラーは ErrorObject を返す

undefined をエラー用途に使わない。

---

## 📌 将来の注意点

- 未定義変数アクセスはどう扱うか（エラー or null）  
- 関数の戻り値が無い場合は null を返す  
- UserObject の has(key) 実装をメソッドテーブル方式に統一する  
- TryGetProperty の責務を全型で統一する（別メモ参照）

---

## 🎉 まとめ

Tsumugi Script は **undefined を導入しない** ことで：

- 言語仕様のシンプルさ  
- 実装の軽さ  
- ユーザーの理解しやすさ  
- バグの少なさ  

を維持する。

この方針は長期的な設計としても合理的であり、  
将来のリファクタリング時にもこのメモを参照すること。

# Tsumugi UE5 – Todo フォルダ README

このフォルダは、Tsumugi Script / Runtime / Editor 拡張に関する  
**将来の改善案・仕様メモ・リファクタリング候補** を保存するための場所です。

実装予定ではあるが「今すぐではないもの」や、  
将来の大規模リファクタリングに備えた設計メモをここにまとめます。

---

## 📁 このフォルダに置くべきもの

- 言語仕様に関する将来の改善案  
- ランタイム内部のリファクタリング計画  
- 未実装機能の仕様メモ  
- 実装方針の比較・検討資料  
- 設計上の判断理由を記録したドキュメント  
- 「後でやるべきだが今はやらない」タスク

---

## 📄 ファイル命名規則

以下のルールに従うと、後から検索しやすくなります。

### 1. 先頭に `tsumugi-` を付ける（プロジェクト名）
例：
```
tsumugi-object-methods-unification-refactor.md
```

### 2. 内容を短く明確に表すキーワードを入れる
- `object-methods`
- `undefined-policy`
- `super-implementation`
- `property-access`
- `runtime-architecture`

### 3. 最後に用途を示すサフィックスを付ける
- `-refactor.md`（リファクタ案）
- `-policy.md`（仕様方針）
- `-design.md`（設計メモ）
- `-notes.md`（雑多なメモ）

例：
```
tsumugi-no-undefined-policy.md
tsumugi-super-method-error-handling.md
tsumugi-object-methods-unification-refactor.md
```

---

## 📌 このフォルダに既に追加したメモ（例）

- `tsumugi-object-methods-unification-refactor.md`  
  → TryGetProperty の責務統一とメソッドテーブル方式への移行案

- `tsumugi-super-method-error-handling.md`  
  → super.method の未実装エラー処理に関するタスク

- `tsumugi-no-undefined-policy.md`  
  → undefined を導入しない方針の仕様メモ

---

## 🧭 運用ルール

- **実装が始まったら Todo から移動する**  
  → `Documents/Design` や `Documents/Specs` へ

- **議論中の仕様はここに置く**  
  → 決定したら別フォルダへ昇格

- **必ず Markdown で書く**  
  → コードブロックや例を入れやすいため

- **1 ファイル 1 テーマ**  
  → 1 つのメモに複数の話題を混ぜない

---

## 🎉 まとめ

このフォルダは Tsumugi Script の  
**「未来のための設計メモ置き場」** です。

- 今すぐ実装しないもの  
- 将来の大規模リファクタリング  
- 仕様の判断理由  
- 言語デザインの方向性  

これらを体系的に残すことで、  
後から見返したときに迷わず開発を進められるようにします。

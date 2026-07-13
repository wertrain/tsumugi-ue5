#pragma once

#include <memory>
#include "TsumugiEngine/Script/Analyzer/ScriptMetadata.h"

namespace tsumugi::script::ast { class Program; }
namespace tsumugi::script::ast { class Attribute; }
namespace tsumugi::script::ast { class IStatement; }
namespace tsumugi::script::object { class Environment; }
namespace tsumugi::script::evaluator { class Evaluator; }

namespace tsumugi::script::analyzer {

/// <summary>
/// スクリプト内の「属性（@attribute）」を解析し、
/// クラス / 関数 / 変数に付与されたメタ情報を抽出するためのコンポーネント。
///
/// AttributeAnalyzer は AST を走査し、
///   - ClassStatement
///   - FunctionStatement
///   - LetStatement
/// に付与された AttributeNode を収集して ScriptMetadata に変換する。
/// （現在はグローバル空間のみで、クラス内、関数内の定義に関しては取得付加）
///
/// 属性の引数は Evaluator を用いて評価されるため、
/// 例えば @range(0, 100) のような式を含む属性でも、
/// 実際の値（IntegerObject など）として取得できる。
///
/// このクラスは「属性の意味付け」を行う層であり、
/// パーサが構築した AST からメタ情報を抽出する役割を担う。
/// 実行フェーズとは独立しており、
/// 主にエディタ連携・コード生成・型情報抽出などの用途で利用される。
/// </summary>
class AttributeAnalyzer {
public:
    /// <summary>
    /// AST Program を走査し、クラス / 関数 / 変数に付与された属性を解析する。
    /// 属性の引数は Evaluator によって評価され、実際の IObject として取得される。
    /// </summary>
    /// <param name="program">走査するプログラム</param>
    /// <param name="environment"></param>
    void Analyze(const ast::Program* program, const std::shared_ptr<object::Environment>& environment);
    void Analyze(const ast::Program* program, evaluator::Evaluator* evaluator, const std::shared_ptr<object::Environment>& environment);

    const std::vector<ClassMetadata>& GetClasses() const  { return classes_; }
    const std::vector<FunctionMetadata>& GetFunctions() const { return functions_; }
    const std::vector<VariableMetadata>& GetVariables() const { return variables_; }

private:
    std::vector<ClassMetadata> classes_;
    std::vector<FunctionMetadata> functions_;
    std::vector<VariableMetadata> variables_;
};

}

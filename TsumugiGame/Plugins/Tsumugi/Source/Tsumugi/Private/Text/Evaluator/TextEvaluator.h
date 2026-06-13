#pragma once

#include "Foundation/Types.h"
#include "Text/Evaluator/IScriptRuntime.h"
#include "Text/Evaluator/CommandRegistry.h"
#include "Script/Objects/Environment.h"
#include <vector>
#include <unordered_map>

namespace tsumugi::text::ast { class IStatement; }
namespace tsumugi::text::ast { class Program; }
namespace tsumugi::script::object { class IObject; }

namespace tsumugi::text::evaluator {

class Evaluator : public IScriptRuntime {
public:
    Evaluator(context::IGameContext& context);

    /// <summary>
    /// スクリプトを開始する
    /// </summary>
    /// <param name="program"></param>
    void Start(const ast::Program& program);

    /// <summary>
    /// 開始したスクリプトを一つ進行する
    /// </summary>
    bool Step();

    /// <summary>
    /// スクリプトを最後まで実行する（デバッグ用）
    /// </summary>
    /// <param name="program"></param>
    void Execute(const ast::Program& program);

    // --- IScriptRuntime の実装 ---
    void JumpToLabel(const tstring& label) override;
    void CallSubroutine(const tstring& label) override;
    void ReturnFromSubroutine() override;
    void RequestStop() override;
    void CancelStop() override;
    bool IsStopRequested() const override;
    void PushBlockState(bool initial) override;
    bool GetBlockState() const override;
    void SetBlockState(bool value) override;
    void PopBlockState() override;
    int GetPC() const override { return pc_; }
    void SetPC(int pc) override { pc_ = pc; }
    void AdvancePC() override { ++pc_; }
    void BeginLink(const tstring& target) override;
    void AppendLinkText(const tstring& text) override;
    void EndLink() override;
    bool IsLinkPending() const override;
    const text::ast::IStatement* GetStatement(int pc) const;
    void SkipUntil(const text::parser::BlockTagDefinition& block) override;
    void ExpandMacro(const tstring& name);
    std::shared_ptr<tsumugi::script::object::IObject> ExecuteScript(const tstring& script) override;

private:
    context::IGameContext& context_;
    CommandRegistry registry_;
    const text::ast::Program* program_;
    int pc_;
    bool stopRequested_;
    std::shared_ptr<tsumugi::script::object::Environment> environment_;

    std::vector<int> callStack_;
    std::unordered_map<tstring, int> labelTable_;
    std::unordered_map<tstring, std::vector<std::unique_ptr<ast::IStatement>>> macroTable_;
    std::vector<bool> blockStateStack_;

    bool linkPending_;
    tstring linkTarget_;
    std::vector<tstring> linkBuffer_;
};

}

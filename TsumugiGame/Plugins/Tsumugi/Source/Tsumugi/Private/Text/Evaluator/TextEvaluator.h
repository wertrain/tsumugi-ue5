#pragma once

#include "Foundation/Types.h"
#include "Text/Evaluator/IScriptRuntime.h"
#include "Text/Evaluator/CommandRegistry.h"
#include <vector>
#include <unordered_map>

namespace tsumugi::text::ast { class IStatement; }
namespace tsumugi::text::ast { class Program; }
namespace tsumugi::text::context { class IGameContext; }

namespace tsumugi::text::evaluator {

class Evaluator : public IScriptRuntime {
public:
    Evaluator(context::IGameContext& context);

    void Execute(const ast::Program& program);

    // --- IScriptRuntime の実装 ---
    void JumpToLabel(const tstring& label) override;
    void CallSubroutine(const tstring& label) override;
    void ReturnFromSubroutine() override;
    int GetPC() const override { return pc_; }
    void SetPC(int pc) override { pc_ = pc; }
    void ExpandMacro(const tstring& name);

private:
    context::IGameContext& context_;
    CommandRegistry registry_;
    int pc_;

    std::vector<int> callStack_;
    std::unordered_map<tstring, int> labelTable_;
    std::unordered_map<tstring, std::vector<std::unique_ptr<ast::IStatement>>> macroTable_;
};

}

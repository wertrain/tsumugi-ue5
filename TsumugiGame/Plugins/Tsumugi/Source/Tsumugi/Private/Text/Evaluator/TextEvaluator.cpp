#include "Text/Evaluator/TextEvaluator.h"
#include "Text/Context/IGameContext.h"
#include "Text/Commands/ICommand.h"
#include "Text/AST/ITextStatement.h"
#include "Text/AST/Statements/LabelStatement.h"
#include "Text/AST/Statements/TextStatement.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/AST/TextProgram.h"

namespace tsumugi::text::evaluator {

Evaluator::Evaluator(context::IGameContext& context)
    : context_(context)
    , registry_()
    , pc_(0)
{
}

void Evaluator::Execute(const ast::Program& program) {

    // ラベルテーブルの構築
    for (int i = 0; i < program.GetStatementCount(); i++) {
        if (program.GetStatement(i)->GetNodeType() == ast::NodeType::kLabelStatement) {
            auto label = static_cast<const ast::statement::LabelStatement*>(program.GetStatement(i));
            labelTable_[label->GetLabelName()] = i;
        }
    }

    pc_ = 0;

    // 実行ループ
    while (pc_ < program.GetStatementCount()) {

        auto statement = program.GetStatement(pc_);
        // タグ
        if (statement->GetNodeType() == ast::NodeType::kTagStatement) {
            auto tag = static_cast<const ast::statement::TagStatement*>(statement);
            if (auto* cmd = registry_.Get(tag->GetTagName())) {
                cmd->Execute(*tag, *this, context_);
            }
            pc_++;
            continue;
        }
        // テキスト
        else if (statement->GetNodeType() == ast::NodeType::kTextStatement) {
            auto text = static_cast<const ast::statement::TextStatement*>(statement);
            context_.ShowText(text->GetText());
            pc_++;
            continue;
        }
        pc_++;
    }
}

void Evaluator::JumpToLabel(const tstring& label) {

    auto it = labelTable_.find(label);
    if (it != labelTable_.end()) {
        pc_ = it->second;
    }
}

void Evaluator::CallSubroutine(const tstring& label) {

    callStack_.push_back(pc_);
    JumpToLabel(label);
}

void Evaluator::ReturnFromSubroutine() {

    if (!callStack_.empty()) {
        pc_ = callStack_.back() + 1;
        callStack_.pop_back();
    }
}

void Evaluator::ExpandMacro(const tstring& name) {

}

}

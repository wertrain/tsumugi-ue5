#include "Text/Evaluator/TextEvaluator.h"
#include "Text/Context/IGameContext.h"
#include "Text/Commands/ICommand.h"
#include "Text/AST/ITextStatement.h"
#include "Text/AST/Statements/LabelStatement.h"
#include "Text/AST/Statements/TextStatement.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/AST/Statements/ScriptBlockStatement.h"
#include "Text/AST/TextProgram.h"

#include "Script/Lexer/ScriptLexer.h"
#include "Script/Parser/ScriptParser.h"
#include "Script/Evaluator/Evaluator.h"
#include "Script/AST/Program.h"
#include "Script/Objects/Environment.h"
#include "Script/Objects/UserObject.h"

namespace tsumugi::text::evaluator {

Evaluator::Evaluator(context::IGameContext& context)
    : context_(context)
    , registry_()
    , program_(nullptr)
    , pc_(0)
    , stopRequested_(false)
    , environment_(std::make_shared<tsumugi::script::object::Environment>())
    , callStack_()
    , labelTable_()
    , macroTable_()
    , blockStateStack_() {

    // 基本変数の登録
    // ゲーム変数であれば f システム変数であれば sf 一時変数であれば tf
    auto user = std::make_shared<tsumugi::script::object::UserObject>();
    environment_->Set(TT("f"), user);
    auto systemUser = std::make_shared<tsumugi::script::object::UserObject>();
    environment_->Set(TT("sf"), systemUser);
    auto tempUser = std::make_shared<tsumugi::script::object::UserObject>();
    environment_->Set(TT("tf"), tempUser);
}

void Evaluator::Start(const ast::Program& program) {

    program_ = &program;

    // ラベルテーブルの構築
    for (int i = 0; i < program.GetStatementCount(); i++) {
        if (program.GetStatement(i)->GetNodeType() == ast::NodeType::kLabelStatement) {
            auto label = static_cast<const ast::statement::LabelStatement*>(program.GetStatement(i));
            labelTable_[label->GetLabelName()] = i;
        }
    }
    pc_ = 0;
}


bool Evaluator::Step() {

    if (!program_) return false;
    if (stopRequested_) return false;
    if (pc_ < 0 || pc_ >= program_->GetStatementCount()) return false;
    if (context_.IsWaiting()) return true;

    auto statement = program_->GetStatement(pc_);

    // タグ
    if (statement->GetNodeType() == ast::NodeType::kTagStatement) {
        auto tag = static_cast<const ast::statement::TagStatement*>(statement);
        command::TagAttributeResolver resolver(*tag, *this);
        if (auto* cmd = registry_.Get(tag->GetTagName())) {
            cmd->Execute(resolver, *this, context_);
        }
    // インラインスクリプト
    } else if (statement->GetNodeType() == ast::NodeType::kScriptBlockStatement) {
        auto script = static_cast<const ast::statement::ScriptBlockStatement*>(statement);
        ExecuteScript(script->GetScriptText());
    // テキスト
    } else if (statement->GetNodeType() == ast::NodeType::kTextStatement) {
        auto text = static_cast<const ast::statement::TextStatement*>(statement);
        context_.ShowText(text->GetText());
    }

    // 新しくウェイトが発生した場合は、 pc_ を進めずにここで終了する（現在の pc_ を維持して次フレームへ）。
    // ジャンプ系コマンドで pc_ が書き換わった場合も、IsWaiting が false ならそのまま進む。
    if (context_.IsWaiting()) {
        return true;
    }

    // ジャンプ補正方式：基本はここで 1 進める
    pc_++;

    return true;
}

void Evaluator::Execute(const ast::Program& program) {

    Start(program);

    // 実行ループ
    while (pc_ < program.GetStatementCount()) {

        if (stopRequested_) {
            break;
        }
        auto statement = program.GetStatement(pc_);
        // タグ
        if (statement->GetNodeType() == ast::NodeType::kTagStatement) {
            auto tag = static_cast<const ast::statement::TagStatement*>(statement);
            command::TagAttributeResolver resolver(*tag, *this);
            if (auto* cmd = registry_.Get(tag->GetTagName())) {
                cmd->Execute(resolver, *this, context_);
            }
        }
        // インラインスクリプト
        else if (statement->GetNodeType() == ast::NodeType::kScriptBlockStatement) {
            auto script = static_cast<const ast::statement::ScriptBlockStatement*>(statement);
            ExecuteScript(script->GetScriptText());
        }
        // テキスト
        else if (statement->GetNodeType() == ast::NodeType::kTextStatement) {
            auto text = static_cast<const ast::statement::TextStatement*>(statement);
            context_.ShowText(text->GetText());
        }
        pc_++;
    }
}

void Evaluator::JumpToLabel(const tstring& label) {

    auto it = labelTable_.find(label);
    if (it != labelTable_.end()) {
        pc_ = it->second - 1;  // 次のループで pc_++ されるので -1
    }
}

void Evaluator::CallSubroutine(const tstring& label) {

    callStack_.push_back(pc_);
    JumpToLabel(label);
}

void Evaluator::ReturnFromSubroutine() {

    if (!callStack_.empty()) {
        int returnPc = callStack_.back();
        callStack_.pop_back();
        pc_ = returnPc - 1;  // 次のステップの pc_++ で「コールタグの次の行」に進めたいので、コールタグ自身のインデックス（returnPc）の 1 つ前に戻す
    }
}

void Evaluator::RequestStop() {

    stopRequested_ = true;
}

bool Evaluator::IsStopRequested() const {

    return stopRequested_;
}

void Evaluator::PushBlockState(bool initial) {

    blockStateStack_.push_back(initial);
}

bool Evaluator::GetBlockState() const {

    if (blockStateStack_.empty()) return false;
    return blockStateStack_.back();
}

void Evaluator::SetBlockState(bool value) {

    if (!blockStateStack_.empty()) {
        blockStateStack_.back() = value;
    }
}

void Evaluator::PopBlockState() {

    if (!blockStateStack_.empty()) {
        blockStateStack_.pop_back();
    }
}

const text::ast::IStatement* Evaluator::GetStatement(int pc) const {

    if (!program_) return nullptr;
    if (pc < 0 || pc >= program_->GetStatementCount()) return nullptr;
    return program_->GetStatement(pc);
}

void Evaluator::SkipUntil(const text::parser::BlockTagDefinition& block)
{
    int pc = this->pc_;

    int nest = 0;

    while (true) {
        pc++;
        this->pc_ = pc;

        auto statement = GetStatement(pc);
        if (!statement) return;

        const text::ast::statement::TagStatement* tag = nullptr;
        if (statement->GetNodeType() == text::ast::NodeType::kTagStatement) {
            tag = static_cast<const text::ast::statement::TagStatement*>(statement);
        }
        if (!tag) continue;
        const auto& name = tag->GetTagName();

        if (name == block.begin) {
            nest++;
        } else if (name == block.end) {
            if (nest == 0) {
                // コマンド実行時、必ず pc を進める実装なので、ここでは指定ブロックの一つ前を指しておく
                this->pc_ = pc - 1;
                return;
            }
            nest--;
        } else if (nest == 0) {
            for (auto& mid : block.middles) {
                if (name == mid) {
                    // 同上
                    this->pc_ = pc - 1;
                    return;
                }
            }
        }
    }
}

void Evaluator::ExpandMacro(const tstring& name) {

}

std::shared_ptr<tsumugi::script::object::IObject> Evaluator::ExecuteScript(const tstring& script) {

    auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(script.c_str());
    auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
    auto root = parser->ParseProgram();

    if (parser->GetLogger().HasAnyLog()) {
        return nullptr; // パースエラーは無視
    }

    tsumugi::script::evaluator::Evaluator evaluator;
    return evaluator.Eval(root.get(), environment_);
}

}

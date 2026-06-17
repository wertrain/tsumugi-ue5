#include "Text/Evaluator/CommandRegistry.h"
#include "Text/Commands/WaitCommand.h"
#include "Text/Commands/ClearTextCommand.h"
#include "Text/Commands/FontCommand.h"
#include "Text/Commands/DelayCommand.h"
#include "Text/Commands/NewLineCommand.h"
#include "Text/Commands/PageBreakCommand.h"
#include "Text/Commands/EvalCommand.h"
#include "Text/Commands/EmbCommand.h"
#include "Text/Commands/IfCommand.h"
#include "Text/Commands/ElsifCommand.h"
#include "Text/Commands/ElseCommand.h"
#include "Text/Commands/EndIfCommand.h"
#include "Text/Commands/EndBlockCommand.h"
#include "Text/Commands/CallCommand.h"
#include "Text/Commands/ReturnCommand.h"
#include "Text/Commands/StopCommand.h"
#include "Text/Commands/JumpCommand.h"
#include "Text/Commands/LineWaitCommand.h"
#include "Text/Commands/LinkCommand.h"
#include "Text/Commands/EndLinkCommand.h"
#include "Text/Commands/IgnoreCommand.h"
#include "Text/Commands/ImageCommand.h"

namespace tsumugi::text::evaluator {

CommandRegistry::CommandRegistry()
    : registry_() {

    registry_[TT("wait")] = std::make_unique<command::WaitCommand>();
    registry_[TT("cm")] = std::make_unique<command::ClearTextCommand>();
    registry_[TT("l")] = std::make_unique<command::LineWaitCommand>();
    registry_[TT("r")] = std::make_unique<command::NewLineCommand>();
    registry_[TT("p")] = std::make_unique<command::PageBreakCommand>();
    registry_[TT("font")] = std::make_unique<command::FontCommand>();
    registry_[TT("delay")] = std::make_unique<command::DelayCommand>();
    registry_[TT("eval")] = std::make_unique<command::EvalCommand>();
    registry_[TT("emb")] = std::make_unique<command::EmbCommand>();
    registry_[TT("if")] = std::make_unique<command::IfCommand>();
    registry_[TT("elsif")] = std::make_unique<command::ElsifCommand>();
    registry_[TT("else")] = std::make_unique<command::ElseCommand>();
    registry_[TT("endif")] = std::make_unique<command::EndIfCommand>();
    registry_[TT("call")] = std::make_unique<command::CallCommand>();
    registry_[TT("return")] = std::make_unique<command::ReturnCommand>();
    registry_[TT("s")] = std::make_unique<command::StopCommand>();
    registry_[TT("jump")] = std::make_unique<command::JumpCommand>();
    registry_[TT("link")] = std::make_unique<command::LinkCommand>();
    registry_[TT("endlink")] = std::make_unique<command::EndLinkCommand>();
    registry_[TT("ignore")] = std::make_unique<command::IgnoreCommand>();
    registry_[TT("image")] = std::make_unique<command::ImageCommand>();
}

CommandRegistry::~CommandRegistry() {

}

command::ICommand* CommandRegistry::Get(const tstring& name) const {

    auto it = registry_.find(name);
    return it != registry_.end() ? it->second.get() : nullptr;
}

}

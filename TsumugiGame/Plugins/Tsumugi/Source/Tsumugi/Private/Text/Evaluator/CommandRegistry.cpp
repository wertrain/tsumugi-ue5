#include "Text/Evaluator/CommandRegistry.h"
#include "Text/Commands/WaitCommand.h"
#include "Text/Commands/ClearTextCommand.h"

namespace tsumugi::text::evaluator {

CommandRegistry::CommandRegistry()
    : registry_() {

    registry_[TT("wait")] = std::make_unique<command::WaitCommand>();
    registry_[TT("cm")] = std::make_unique<command::ClearTextCommand>();
}

CommandRegistry::~CommandRegistry() {

}

command::ICommand* CommandRegistry::Get(const tstring& name) const {

    auto it = registry_.find(name);
    return it != registry_.end() ? it->second.get() : nullptr;
}

}

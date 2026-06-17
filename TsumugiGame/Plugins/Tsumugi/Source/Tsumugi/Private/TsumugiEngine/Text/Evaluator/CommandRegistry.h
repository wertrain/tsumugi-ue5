#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include <unordered_map>

namespace tsumugi::text::command { class ICommand; }

namespace tsumugi::text::evaluator {

class CommandRegistry {
public:
    CommandRegistry();
    ~CommandRegistry();
    command::ICommand* Get(const tstring& name) const;

private:
    std::unordered_map<tstring, std::unique_ptr<command::ICommand>> registry_;
};

}

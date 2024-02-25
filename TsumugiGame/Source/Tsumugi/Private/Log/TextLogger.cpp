#include "Log/TextLogger.h"
#include <iostream>

namespace tsumugi::log {

const char* tsumugi::log::tlognewline = "\n";

void DefaultConsole::WriteLine(const tlogchar* text) {

    std::cout << text << std::endl;
}

}

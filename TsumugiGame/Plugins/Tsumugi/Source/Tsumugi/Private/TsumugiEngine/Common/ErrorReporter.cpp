#include "TsumugiEngine/Common/ErrorReporter.h"

namespace tsumugi::common {

ErrorReporter::ErrorReporter() {

}

std::string ErrorReporter::ToString(script::lexer::TokenType t) {

    return script::lexer::TokenTypeToString(t);
}

std::string ErrorReporter::ToString(script::object::ObjectType t) {

    return script::object::ObjectTypeToString(t);
}

}

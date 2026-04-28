#include "Common/ErrorReporter.h"

namespace tsumugi::common {

ErrorReporter::ErrorReporter() {

}

std::string ErrorReporter::ToString(script::lexing::TokenType t) {

    return script::lexing::TokenTypeToString(t);
}

std::string ErrorReporter::ToString(script::object::ObjectType t) {

    return script::object::ObjectTypeToString(t);
}

}

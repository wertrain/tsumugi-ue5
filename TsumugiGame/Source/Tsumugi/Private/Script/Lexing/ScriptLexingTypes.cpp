#include "Script/Lexing/ScriptLexingTypes.h"

namespace tsumugi::script::lexing {

LexingPosition::LexingPosition() :
    position_(0),
    lines_(0),
    columns_(0) {

}

LexingPosition::LexingPosition(const int position, const int lines, const int columns) :
    position_(position),
    lines_(lines),
    columns_(columns) {

}

int LexingPosition::GetPosition() const {
    return position_;
}

void LexingPosition::SetPosition(const int position) {
    position_ = position;
}

int LexingPosition::GetLines() const {
    return lines_;
}

void LexingPosition::SetLines(int lines) {
    lines_ = lines;
}

int LexingPosition::GetColumns() const {
    return columns_;
}

void LexingPosition::SetColumns(int columns) {
    columns_ = columns;
}

}

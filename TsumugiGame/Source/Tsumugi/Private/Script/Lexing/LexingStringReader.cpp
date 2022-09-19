#include "Script/Lexing/LexingStringReader.h"

namespace tsumugi::script::lexing {

LexingStringReader::LexingStringReader(const char* string) :
    position_(0),
    lines_(0),
    columns_(0),
    string_(string)  {

}

int LexingStringReader::GetPosition() const {
    return position_;
}

void LexingStringReader::SetPosition(const int position) {
    position_ = position;
}

int LexingStringReader::GetLines() const {
    return lines_;
}

void LexingStringReader::SetLines(int lines) {
    lines_ = lines;
}

int LexingStringReader::GetColumns() const {
    return columns_;
}

void LexingStringReader::SetColumns(int columns) {
    columns_ = columns;
}

char LexingStringReader::Peek() const {
    return Peek(0);
}

char LexingStringReader::Peek(int offset) const {

    int position = position_ + offset;
    if (0 <= position && string_.length() > position) {
        return string_[position];
    }
    return -1;
}

char LexingStringReader::Read() {

    if (string_.length() > position_) {
        SkipNewLine();
        return string_[position_++];
    }
    return -1;
}

int LexingStringReader::Seek(int offset) {
    return Seek(offset, SeekOrigin::kBegin);
}

int LexingStringReader::Seek(int offset, SeekOrigin origin) {
    
    // TODO: s”A—ñ”‚ÌÄŒvŽZ

    switch (origin) {
    case SeekOrigin::kBegin:
        if (0 > string_.length() + offset || string_.length() <= offset) {
            return position_;
        }
        position_ = offset;
        break;

    case SeekOrigin::kCurrent:
        if (0 > string_.length() + offset || string_.length() <= position_ + offset) {
            return position_;
        }
        position_ = position_ + offset;
        break;

    case SeekOrigin::kEnd:
        if (0 > string_.length() + offset || string_.length() <= string_.length() + offset) {
            return position_;
        }
        position_ = string_.length() + offset;
        break;
    }
    return position_;
}

void LexingStringReader::SkipNewLine() {

    if (string_[position_] == '\n') {
        ++lines_;
        columns_ = 0;
    } else {
        ++columns_;
    }
}

}

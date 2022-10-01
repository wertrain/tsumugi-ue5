#include "Script/Lexing/LexingStringReader.h"

namespace tsumugi::script::lexing {

LexingStringReader::LexingStringReader() :
    LexingStringReader(TT_EMPTY()) {

}

LexingStringReader::LexingStringReader(const tchar* string) :
    position_(0),
    lines_(0),
    columns_(0),
    string_(string)  {

}

void LexingStringReader::SetString(const tchar* string) {
    string_ = string;
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

tchar LexingStringReader::Peek() const {
    return Peek(0);
}

tchar LexingStringReader::Peek(int offset) const {

    int position = position_ + offset;
    if (0 <= position && string_.length() > position) {
        return string_[position];
    }
    return -1;
}

tchar LexingStringReader::Read() {

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

    if (string_[position_] == TT('\n')) {
        ++lines_;
        columns_ = 0;
    } else {
        ++columns_;
    }
}

LexingPosition LexingStringReader::GetLexingPosition() const {
    return LexingPosition(position_, lines_, columns_);
}

}

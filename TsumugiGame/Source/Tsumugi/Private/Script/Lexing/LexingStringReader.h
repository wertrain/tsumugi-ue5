#pragma once

#include "Foundation/Types.h"

namespace tsumugi::script::lexing {

/// <summary>
/// �����͗p�� StringReader �N���X
/// </summary>
class LexingStringReader {
public:
    enum class SeekOrigin {
        kBegin,
        kCurrent,
        kEnd
    };

public:
    LexingStringReader(const char* string);

    int GetPosition() const;
    void SetPosition(int position);
    int GetLines() const;
    void SetLines(int lines);
    int GetColumns() const;
    void SetColumns(int columns);

    char Peek() const;
    char Peek(int offset) const;
    char Read();
    int Seek(int offset);
    int Seek(int offset, SeekOrigin origin);

private:
    void SkipNewLine();

private:
    int position_;
    int lines_;
    int columns_;
    tstring string_;
};

}

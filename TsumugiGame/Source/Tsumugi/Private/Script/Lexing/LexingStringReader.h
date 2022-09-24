#pragma once

#include "Foundation/Types.h"

namespace tsumugi::script::lexing {

/// <summary>
/// š‹å‰ğÍ—p‚Ì StringReader ƒNƒ‰ƒX
/// </summary>
class LexingStringReader {
public:
    enum class SeekOrigin {
        kBegin,
        kCurrent,
        kEnd
    };

public:
    LexingStringReader();
    LexingStringReader(const tchar* string);

    void SetString(const tchar* string);
    int GetPosition() const;
    void SetPosition(int position);
    int GetLines() const;
    void SetLines(int lines);
    int GetColumns() const;
    void SetColumns(int columns);

    tchar Peek() const;
    tchar Peek(int offset) const;
    tchar Read();
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

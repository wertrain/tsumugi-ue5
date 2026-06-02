#pragma once

#include "Foundation/Types.h"
#include "Script/Lexer/ScriptLexingTypes.h"

namespace tsumugi::script::lexer {

/// <summary>
/// 蟄怜唱隗｣譫千畑縺ｮ StringReader 繧ｯ繝ｩ繧ｹ
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

    LexingPosition GetLexingPosition() const;

private:
    void SkipNewLine();

private:
    int position_;
    int lines_;
    int columns_;
    tstring string_;
};

}

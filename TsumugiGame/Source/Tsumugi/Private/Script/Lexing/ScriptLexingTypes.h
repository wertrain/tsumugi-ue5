#pragma once

#include "Foundation/Types.h"

namespace tsumugi::script::lexing {

/// <summary>
/// âêÕà íu
/// </summary>
class LexingPosition {
public:
    LexingPosition();
    LexingPosition(int position, int lines, int columns);

    int GetPosition() const;
    void SetPosition(int position);
    int GetLines() const;
    void SetLines(int lines);
    int GetColumns() const;
    void SetColumns(int columns);

private:
    int position_;
    int lines_;
    int columns_;
};

}

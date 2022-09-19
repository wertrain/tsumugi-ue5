#pragma once

namespace tsumugi::script::lexing {

/// <summary>
/// ��͈ʒu
/// </summary>
class LexingPosition {
public:
    LexingPosition();

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

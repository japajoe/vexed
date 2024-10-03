#include "icursor.h"
#include <string.h>

namespace vexed {
    ICursor::ICursor() {
        setCursorIndex(0, 0);
        blinkTimer = 0.0f;
        blinkInterval = 0.25f;
        lastKeyStroke = 0.0f;
    }

    ICursor::~ICursor() {

    }

    void ICursor::resetLastKeyStroke() {
        lastKeyStroke = Application::getInstance()->getTime();
        blinkTimer = 0;
    }

    size_t ICursor::getCursorIndex() const {
        return cursorIndex;
    }

    void ICursor::setCursorIndex(size_t index, size_t textSize) {
        if(index > textSize) {
            index = textSize;
        }

        cursorIndex = index;
    }

    void ICursor::getCursorPosition(const std::string &text, size_t &row, size_t &column) const {
        row = 0;
        column = 0;
        size_t index = 0;

        // Iterate through the text to find the current row and column
        while (index < cursorIndex && index < text.size()) {
            if (text[index] == '\n') {
                ++row; // Move to the next row
                column = 0; // Reset column count
            } else {
                ++column; // Move to the next character in the current line
            }
            ++index; // Move to the next character
        }
    }

    void ICursor::setCursorPosition(const std::string &text, size_t row, size_t column) {
        size_t currentRow = 0;
        size_t index = 0;

        // Iterate through the text to find the specified row
        while (currentRow < row && index < text.size()) {
            if (text[index] == '\n') {
                ++currentRow; // Move to the next row
            }
            ++index; // Move to the next character
        }

        // If we have found the desired row, set the cursor
        if (currentRow == row) {
            // If column is out of bounds, set to the end of the line
            size_t lineStart = (currentRow > 0) ? text.rfind('\n', index - 1) + 1 : 0;
            size_t lineEnd = text.find('\n', lineStart);

            if (lineEnd == std::string::npos) {
                lineEnd = text.size(); // Last line goes to end of text
            }

            // Limit the cursor index to the length of the current line
            cursorIndex = lineStart + std::min(column, lineEnd - lineStart);
        } else {
            // If the row exceeds the total number of lines, set to end of text
            cursorIndex = text.size();
        }
    }
}
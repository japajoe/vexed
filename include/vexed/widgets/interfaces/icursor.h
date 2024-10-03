#ifndef VEXED_ICURSOR_H
#define VEXED_ICURSOR_H

#include "../../vexed.h"

namespace vexed {
    class ICursor {
    public:
        ICursor();
        virtual ~ICursor();
    protected:
        size_t cursorIndex;
        size_t cursorRow;
        size_t cursorColumn;
        float blinkTimer;
        float blinkInterval;
        float lastKeyStroke;
        void resetLastKeyStroke();
        size_t getCursorIndex() const;
        void setCursorIndex(size_t index, size_t textSize);
        void getCursorPosition(const std::string &text, size_t &row, size_t &column) const;
        void setCursorPosition(const std::string &text, size_t row, size_t column);
    };
}

#endif
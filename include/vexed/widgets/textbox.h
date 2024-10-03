#ifndef VEXED_TEXTBOX_H
#define VEXED_TEXTBOX_H

#include "widget.h"
#include "interfaces/ifont.h"
#include "interfaces/icursor.h"

namespace vexed {
    class Textbox : public Widget, public IFont, public ICursor {
    public:
        SubmitCallback submit;
        Textbox();
        std::string getText() const;
        void setText(const std::string &text);
        bool isMultiLine() const;
        void setMultiLine(bool multiLine);
    protected:
        void onRender() override;
        void onCharPress(uint32_t codepoint) override;
        void onKeyDown(KeyCode keycode) override;
        void onKeyUp(KeyCode keycode) override;
        void onKeyPress(KeyCode keycode) override;
        void onKeyRepeat(KeyCode keycode) override;
        void onButtonDown(ButtonCode buttoncode) override;
        void onButtonUp(ButtonCode buttoncode) override;
        void onMouseEnter() override;
        void onMouseLeave() override;
    private:
        std::string text;
        bool multiLine;
        Vector2 textOffset;
        void renderTextArea();
        void renderCursor();
        size_t getIndexOfPreviousWord();
        size_t getIndexOfNextWord();
        size_t getIndexOfPreviousNewLineChar();
        size_t getIndexOfNextNewLineChar();
        bool isSpaceOrPunct(char c) const;
        Vector2 calculateCursorPosition();
        Vector2 calculateTextPosition();
    };
}

#endif
#ifndef VEXED_INPUTFLOAT_H
#define VEXED_INPUTFLOAT_H

#include "widget.h"
#include "interfaces/ifont.h"
#include "interfaces/icursor.h"
#include "arrowbutton.h"

namespace vexed {
    class InputFloat : public Widget, public IFont, public ICursor {
    public:
        SubmitCallback submit;
        InputFloat();
        float getValue() const;
        void setValue(float value);
        float getMinValue() const;
        void setMinValue(float value);
        float getMaxValue() const;
        float getIncrement() const;
        void setIncrement(float increment);
        void setMaxValue(float value);
        void setEnterIsSubmit(bool enterIsSubmit);
        bool isEnterSubmit() const;
        size_t getDecimalPlaces() const;
        void setDecimalPlaces(size_t decimalPlaces);
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
        bool containsPoint(const Vector2 &point) override;
    private:
        std::string text;
        float value;
        float valueMin;
        float valueMax;
        float valueIncrement;
        size_t decimalPlaces;
        bool enterIsSubmit;
        Vector2 textOffset;
        float buttonWidth;
        ArrowButton *buttonUp;
        ArrowButton *buttonDown;
        void renderTextArea();
        void renderCursor();
        size_t getIndexOfPreviousWord();
        size_t getIndexOfNextWord();
        size_t getIndexOfPreviousNewLineChar();
        size_t getIndexOfNextNewLineChar();
        bool isSpaceOrPunct(char c) const;
        Vector2 calculateCursorPosition();
        Vector2 calculateTextPosition();
        void applyValue();
        void truncatDecimalPlaces();
    };
}

#endif
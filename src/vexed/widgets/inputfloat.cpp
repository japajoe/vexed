#include "inputfloat.h"
#include <cmath>
#include <stdexcept>
#include <regex>

namespace vexed {
    InputFloat::InputFloat() : Widget(), IFont(), ICursor() {
        setPosition(Vector2(0, 0));
        setSize(Vector2(100, 24));
        setMinValue(0.0f);
        setMaxValue(1.0f);
        setValue(0.0f);
        setIncrement(0.1f);
        setFontSize(16);
        decimalPlaces = 3;
        buttonWidth = 16;
        textOffset = Vector2(4, 4);
        enterIsSubmit = false;
        buttonUp = add<ArrowButton>();
        buttonDown = add<ArrowButton>();
        buttonUp->setDirection(ArrowDirection::Up);
        buttonDown->setDirection(ArrowDirection::Down);
        buttonUp->setPosition(Vector2(100, 0));
        buttonDown->setPosition(Vector2(100, 0));

        buttonUp->click = [this] (const Widget *widget) {
            float val = getValue() + 1.0f;
            setValue(val);
        };

        buttonDown->click = [this] (const Widget *widget) {
            float val = getValue() - 1.0f;
            setValue(val);
        };
    }

    float InputFloat::getValue() const {
        return value;
    }

    void InputFloat::setValue(float value) {
        if(value < valueMin)
            value = valueMin;
        if(value > valueMax)
            value = valueMax;
        this->value = value;
        text = std::to_string(value);
        truncatDecimalPlaces();
    }

    float InputFloat::getMinValue() const {
        return valueMin;
    }

    void InputFloat::setMinValue(float value) {
        this->valueMin = value;
    }

    float InputFloat::getMaxValue() const {
        return valueMax;
    }

    void InputFloat::setMaxValue(float value) {
        this->valueMax = value;
    }

    float InputFloat::getIncrement() const {
        return valueIncrement;
    }

    void InputFloat::setIncrement(float increment) {
        if(increment == 0.0f)
            increment = 0.1f;
        if(increment < valueMin)
            increment = valueMin;
        if(increment > valueMax)
            increment = valueMax;
        this->valueIncrement = increment;
    }

    void InputFloat::setEnterIsSubmit(bool enterIsSubmit) {
        this->enterIsSubmit = enterIsSubmit;
    }

    bool InputFloat::isEnterSubmit() const {
        return enterIsSubmit;
    }

    size_t InputFloat::getDecimalPlaces() const {
        return decimalPlaces;
    }

    void InputFloat::setDecimalPlaces(size_t decimalPlaces) {
        if(decimalPlaces < 1)
            decimalPlaces = 1;
        this->decimalPlaces = decimalPlaces;
    }

    void InputFloat::onRender() {
        renderTextArea();
        renderCursor();
    }

    void InputFloat::renderTextArea() {
        Vector2 position = getPosition();
        Vector2 size = getSize();
        size.x -= buttonWidth;
        addRectangle(position, size, 0, getColor(WidgetColor_TextboxNormal));
        //addBorder(position, size, 1, Color(1, 1, 1, 0.3), BorderOptions_All);

        Vector2 textPosition = calculateTextPosition();
        Rectangle clippingRect(position.x, position.y, size.x, size.y);
        addText(textPosition, font, false, text, getFontSize(), getColor(WidgetColor_TextboxText), clippingRect);

        buttonUp->setSize(Vector2(buttonWidth, size.y * 0.5));
        buttonDown->setSize(Vector2(buttonWidth, size.y * 0.5));
        buttonUp->setPosition(Vector2(size.x, 0));
        buttonDown->setPosition(Vector2(size.x, size.y * 0.5));
    }

    void InputFloat::renderCursor() {
        if(!isFocused())
            return;

        auto state = getState();

        if(state & WidgetState_Focused) {
            Vector2 cursorPosition = calculateCursorPosition();
            Vector2 cursorSize(2.0f, font->getLineHeight() * (getFontSize() / font->getPixelSize()));

            Vector2 position = getPosition();
            Vector2 size = getSize();
            size.x -= buttonWidth;
            Rectangle clippingRect(position.x, position.y, size.x, size.y);

            Color cursorColor = getColor(WidgetColor_TextboxText);
            
            if(Application::getInstance()->getTime() >= (lastKeyStroke + blinkInterval)) {
                float alpha = (cosf(blinkTimer * 5.0f) + 1.0f) * 0.5f;
                int on = alpha >= 0.5f;
                addRectangle(cursorPosition, cursorSize, 0, Color(cursorColor.r, cursorColor.g, cursorColor.b, on * 0.9f), clippingRect);
                blinkTimer += Application::getInstance()->getDeltaTime();
            } else {
                addRectangle(cursorPosition, cursorSize, 0, Color(cursorColor.r, cursorColor.g, cursorColor.b, 0.9f), clippingRect);
            }
        }
    }

    static bool isValidFloat(const std::string &str) {
        // Allow negative sign at start, digits, one decimal point
        static std::regex floatRegex("^-?\\d*\\.?\\d+$");
        return std::regex_match(str, floatRegex);
    }

    void InputFloat::onCharPress(uint32_t codepoint) {
        if(!isFocused())
            return;

        auto state = getState();

        if((state & WidgetState_Focused) == 0)
            return;

        uint32_t firstChar = font->getCodePointOfFirstChar();
        uint32_t lastChar = firstChar + font->getNumberOfCharacters();
        uint32_t charIndex = codepoint - firstChar;
        if(charIndex > lastChar)
            return;
        
        char c = static_cast<char>(codepoint);

        if (isdigit(c) || c == '.' || c == '-') {
            // Allow a single period
            if (c == '.') {
                if (text.find('.') != std::string::npos) {
                    return; // Reject if a period already exists
                }
            } 
            // Allow leading minus only at the start
            else if (c == '-') {
                if (!text.empty()) {
                    return; // Reject if not at the start
                }
            } 
            // Allow leading zero only followed by a period or more digits
            else {
                if (text.empty() && c == '0') {
                    // Accept "0" as the first character but restrict further input
                    if (cursorIndex == 1) {
                        return; // Reject further input immediately after "0"
                    }
                }
            }

            if (cursorIndex < text.size()) {
                text.insert(cursorIndex, 1, c);
                setCursorIndex(cursorIndex + 1, text.size());
                resetLastKeyStroke();
            } else {
                text.append(1, c);
                setCursorIndex(cursorIndex + 1, text.size());
                resetLastKeyStroke();
            }

            if(!enterIsSubmit)
                applyValue();
        }
    }

    void InputFloat::onKeyDown(KeyCode keycode) {
        if(!isFocused())
            return;

        auto state = getState();

        if((state & WidgetState_Focused) == 0)
            return;
            
        auto keyboard = Application::getInstance()->getKeyboard();
        
        if(keycode == KeyCode::Left) {
            if(keyboard->getKey(KeyCode::LeftControl) || keyboard->getKey(KeyCode::RightControl)) {
                size_t newCursorIndex = getIndexOfPreviousWord();
                setCursorIndex(newCursorIndex, text.size());
                resetLastKeyStroke();
            } else {
                if(cursorIndex >= 1) {
                    setCursorIndex(cursorIndex - 1, text.size());
                    resetLastKeyStroke();
                }
            }
        }

        if(keycode == KeyCode::Right) {
            if(keyboard->getKey(KeyCode::LeftControl) || keyboard->getKey(KeyCode::RightControl)) {
                size_t newCursorIndex = getIndexOfNextWord();
                setCursorIndex(newCursorIndex, text.size());
                resetLastKeyStroke();
            } else {
                if(cursorIndex < text.size()) {
                    setCursorIndex(cursorIndex + 1, text.size());
                    resetLastKeyStroke();
                }
            }
        }

        if(keycode == KeyCode::Up) {
            size_t row, column;
            getCursorPosition(text, row, column);
            if(row > 0) {
                setCursorPosition(text, row - 1, column);
                resetLastKeyStroke();
            } else {
                setCursorPosition(text, 0, 0);
                resetLastKeyStroke();
            }
        }

        if(keycode == KeyCode::Down) {
            size_t row, column;
            getCursorPosition(text, row, column);
            setCursorPosition(text, row + 1, column);
            resetLastKeyStroke();
        }

        if(keycode == KeyCode::Home) {
            size_t newCursorIndex = getIndexOfPreviousNewLineChar();
            setCursorIndex(newCursorIndex, text.size());
            resetLastKeyStroke();
        }

        if(keycode == KeyCode::End) {
            size_t newCursorIndex = getIndexOfNextNewLineChar();
            setCursorIndex(newCursorIndex, text.size());
            resetLastKeyStroke();
        }

        if(keycode == KeyCode::Backspace) {
            if(cursorIndex > 0 && cursorIndex <= text.size() && text.size() > 0) {
                text.erase(cursorIndex - 1, 1);
                setCursorIndex(cursorIndex - 1, text.size());
                resetLastKeyStroke();
                if(!enterIsSubmit)
                    applyValue();
            }
        }

        if(keycode == KeyCode::Delete) {
            if(cursorIndex < text.size()) {
                text.erase(cursorIndex, 1);
                resetLastKeyStroke();
                if(!enterIsSubmit)
                    applyValue();
            }
        }

        if(keycode == KeyCode::Enter || keycode == KeyCode::KeypadEnter) {
            setFocusedWidget(nullptr);
            setState(WidgetState_Focused, false);
            if(text.size() == 0)
                setValue(valueMin);
            if(enterIsSubmit)
                applyValue();
            text = std::to_string(value);
            truncatDecimalPlaces();
            if(submit)
                submit(this);
        }
    }

    void InputFloat::onKeyUp(KeyCode keycode) {

    }

    void InputFloat::onKeyPress(KeyCode keycode) {

    }

    void InputFloat::onKeyRepeat(KeyCode keycode) {
        onKeyDown(keycode);
    }

    void InputFloat::onButtonDown(ButtonCode buttoncode) {
        auto state = getState();

        if(state & WidgetState_Hovered) {
            if(!isFocused()) {
                blinkTimer = 0.0f;
                resetLastKeyStroke();
            }
            setFocusedWidget(this);
            setState(WidgetState_Focused, true);
            setState(WidgetState_Pressed, true);
        } else {
            if(isFocused()) {
                setFocusedWidget(nullptr);
                setState(WidgetState_Focused, false);
                setState(WidgetState_Pressed, false);                
            }
        }
    }

    void InputFloat::onButtonUp(ButtonCode buttoncode) {
        setState(WidgetState_Pressed, false);

        auto state = getState();

        if((state & WidgetState_Hovered) == 0) {
            if(isFocused()) {
                setFocusedWidget(nullptr);
            }
        }
    }

    void InputFloat::onMouseEnter() {
        setState(WidgetState_Hovered, true);
        setState(WidgetState_Normal, false);
    }

    void InputFloat::onMouseLeave() {
        setState(WidgetState_Hovered, false);
        setState(WidgetState_Normal, true);
    }

    bool InputFloat::containsPoint(const Vector2 &point) {
        auto position = getPosition();
        auto size = getSize();
        size.x -= buttonWidth;
        return (point.x >= position.x && point.x <= position.x + size.x &&
                point.y >= position.y && point.y <= position.y + size.y);
    }

    size_t InputFloat::getIndexOfPreviousWord() {
        if (cursorIndex == 0) {
            return 0; // Already at the beginning
        }

        // Start from the cursor and move backwards
        size_t index = cursorIndex - 1;

        // Move backwards to find the start of the previous word
        while (index > 0 && isSpaceOrPunct(text[index])) {
            --index;
        }

        // Now move backwards to find the start of the word
        while (index > 0 && !isSpaceOrPunct(text[index - 1])) {
            --index;
        }

        return index; // This will be the start of the previous word
    }

    size_t InputFloat::getIndexOfNextWord() {
        if (cursorIndex >= text.size()) {
            return text.size(); // Already at the end
        }

        // Start from the cursor and move forwards
        size_t index = cursorIndex;

        // Move forwards to find the start of the next word
        while (index < text.size() && isSpaceOrPunct(text[index])) {
            ++index;
        }

        // Now move forwards to find the end of the word
        while (index < text.size() && !isSpaceOrPunct(text[index])) {
            ++index;
        }

        return index; // This will be the end of the next word
    }

    size_t InputFloat::getIndexOfPreviousNewLineChar() {
        // Start searching from the cursorIndex - 1 to avoid counting the current position
        for (size_t i = cursorIndex; i > 0; --i) {
            if (text[i - 1] == '\n') {
                return i; // Return the index of the newline character
            }
        }
        return 0; // No previous newline found
    }

    size_t InputFloat::getIndexOfNextNewLineChar() {
        // Start searching from the cursorIndex to the end of the string
        for (size_t i = cursorIndex; i < text.size(); ++i) {
            if (text[i] == '\n') {
                return i; // Return the index of the newline character
            }
        }
        return text.size(); // No next newline found
    }

    bool InputFloat::isSpaceOrPunct(char c) const {
        return std::isspace(c) || std::ispunct(c);
    }

    Vector2 InputFloat::calculateCursorPosition() {
        Vector2 position = getPosition();
        Vector2 size = getSize();
        size.x -= buttonWidth;
        Vector2 cursorPosition(position.x + textOffset.x, position.y + textOffset.y);
        font->computeCursorPosition(text, cursorIndex, getFontSize(), cursorPosition.x, cursorPosition.y);
        float rectangleBottom = position.y + size.y;
        float rectangleRight = position.x + size.x;
        float cursorWidth = 2.0f;
        float cursorHeight = font->getLineHeight() * (fontSize / font->getPixelSize());

        auto absf = [] (float v) {
            if(v < 0.0f)
                return v * -1.0f;
            return v;
        };

        float cursorX = cursorPosition.x + cursorWidth;
        float cursorY = cursorPosition.y + cursorHeight;

        if(cursorY > rectangleBottom) {
            float distanceBelow = absf(cursorY - rectangleBottom); // Distance from bottom of rectangle to cursor
            size_t rowsBelow = static_cast<size_t>(distanceBelow / cursorHeight) + 1; // Number of rows below
            cursorPosition.y -= rowsBelow * cursorHeight;
        }

        if(cursorX > rectangleRight) {
            float distanceRight = absf(cursorX - rectangleRight); // Distance from right edge to cursor
            size_t columnsRight = static_cast<size_t>(distanceRight / cursorWidth) + 3; // Number of columns outside
            cursorPosition.x -= (columnsRight * cursorWidth);
        }

        return cursorPosition;
    }

    Vector2 InputFloat::calculateTextPosition() {
        Vector2 position = getPosition();
        Vector2 size = getSize();
        size.x -= buttonWidth;
        Vector2 cursorPosition(position.x + textOffset.x, position.y + textOffset.y);
        Vector2 textPosition(position.x + textOffset.x, position.y + textOffset.y);
        font->computeCursorPosition(text, cursorIndex, getFontSize(), cursorPosition.x, cursorPosition.y);
        float rectangleBottom = position.y + size.y;
        float rectangleRight = position.x + size.x;
        float cursorWidth = 2.0f;
        float cursorHeight = font->getLineHeight() * (fontSize / font->getPixelSize());

        auto absf = [] (float v) {
            if(v < 0.0f)
                return v * -1.0f;
            return v;
        };

        float cursorX = cursorPosition.x + cursorWidth;
        float cursorY = cursorPosition.y + cursorHeight;

        if(cursorY > rectangleBottom) {
            float distanceBelow = absf(cursorY - rectangleBottom); // Distance from bottom of rectangle to cursor
            size_t rowsBelow = static_cast<size_t>(distanceBelow / cursorHeight) + 1; // Number of rows below
            textPosition.y -= rowsBelow * cursorHeight;
        }

        if(cursorX > rectangleRight) {
            float distanceRight = absf(cursorX - rectangleRight); // Distance from right edge to cursor
            size_t columnsRight = static_cast<size_t>(distanceRight / cursorWidth) + 3; // Number of columns outside
            textPosition.x -= (columnsRight * cursorWidth);
        }

        return textPosition;
    }

    void InputFloat::applyValue() {
        auto parseFloat = [](const std::string& str, float& outValue) -> bool {
            try {
                outValue = std::stof(str);
                return true;
            } catch (const std::invalid_argument&) {
                return false;
            } catch (const std::out_of_range&) {
                return false;
            }
        };

        float newValue = 0.0f;
        if(parseFloat(text, newValue)) {
            if(newValue > valueMax) {
                newValue = valueMax;                
            }
            if(newValue < valueMin) {
                newValue = valueMin;
            }
            this->value = newValue;
            //printf("Value %f\n", this->value);
        }
    }

    void InputFloat::truncatDecimalPlaces() {
        // Find the position of the decimal point
        size_t pos = text.find('.');
        
        // If there's no decimal point, return the original string
        if (pos == std::string::npos) {
            return;
        }

        // Calculate the position to truncate
        size_t truncatePos = pos + decimalPlaces + 1; // +1 for the decimal point

        // Check if we need to truncate
        if (truncatePos < text.length()) {
            text = text.substr(0, truncatePos);
        }
    }
}
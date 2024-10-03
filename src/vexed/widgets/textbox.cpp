#include "textbox.h"
#include <cmath>

namespace vexed {
    Textbox::Textbox() : Widget(), IFont(), ICursor() {
        multiLine = true;
        setPosition(Vector2(0, 0));
        setSize(Vector2(200, 24 * 4));
        setText("Type your text here");
        setFontSize(16);
        textOffset = Vector2(4, 4);
    }

    std::string Textbox::getText() const {
        return text;
    }

    void Textbox::setText(const std::string &text) {
        this->text = text;
    }

    bool Textbox::isMultiLine() const {
        return multiLine;
    }

    void Textbox::setMultiLine(bool multiLine) {
        this->multiLine = multiLine;
    }

    void Textbox::onRender() {
        renderTextArea();
        renderCursor();
    }

    void Textbox::renderTextArea() {
        Vector2 position = getPosition();
        Vector2 size = getSize();
        addRectangle(position, size, 0, getColor(WidgetColor_TextboxNormal));
        //addBorder(position, size, 1, Color(1, 1, 1, 0.3), BorderOptions_All);

        Vector2 textPosition = calculateTextPosition();
        Rectangle clippingRect(position.x, position.y, size.x, size.y);
        addText(textPosition, font, false, text, getFontSize(), getColor(WidgetColor_TextboxText), clippingRect);
    }

    void Textbox::renderCursor() {
        if(!isFocused())
            return;

        auto state = getState();

        if(state & WidgetState_Focused) {
            Vector2 cursorPosition = calculateCursorPosition();
            Vector2 cursorSize(2.0f, font->getLineHeight() * (getFontSize() / font->getPixelSize()));

            Vector2 position = getPosition();
            Vector2 size = getSize();
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

    void Textbox::onCharPress(uint32_t codepoint) {
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

        if (cursorIndex < text.size()) {
            text.insert(cursorIndex, 1, c);
            setCursorIndex(cursorIndex + 1, text.size());
            resetLastKeyStroke();
        } else {
            text.append(1, c);
            setCursorIndex(cursorIndex + 1, text.size());
            resetLastKeyStroke();
        }
    }

    void Textbox::onKeyDown(KeyCode keycode) {
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
            }
        }

        if(keycode == KeyCode::Delete) {
            if(cursorIndex < text.size()) {
                text.erase(cursorIndex, 1);
                resetLastKeyStroke();
            }
        }

        if(keycode == KeyCode::Enter) {
            if(multiLine) {
                text.insert(cursorIndex, 1, '\n');
                setCursorIndex(cursorIndex + 1, text.size());
                resetLastKeyStroke();
            } else {
                setFocusedWidget(nullptr);
                setState(WidgetState_Focused, false);
                submit(this);
            }
        }

        if(keycode == KeyCode::Tab) {
            if(keyboard->getKey(KeyCode::LeftShift) || keyboard->getKey(KeyCode::RightShift)) {

            } else {
                text.insert(cursorIndex, 4, ' ');
                setCursorIndex(cursorIndex + 4, text.size());
                resetLastKeyStroke();
            }
        }
    }

    void Textbox::onKeyUp(KeyCode keycode) {

    }

    void Textbox::onKeyPress(KeyCode keycode) {

    }

    void Textbox::onKeyRepeat(KeyCode keycode) {
        onKeyDown(keycode);
    }

    void Textbox::onButtonDown(ButtonCode buttoncode) {
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

    void Textbox::onButtonUp(ButtonCode buttoncode) {
        setState(WidgetState_Pressed, false);

        auto state = getState();

        if((state & WidgetState_Hovered) == 0) {
            if(isFocused()) {
                setFocusedWidget(nullptr);
            }
        }
    }

    void Textbox::onMouseEnter() {
        setState(WidgetState_Hovered, true);
        setState(WidgetState_Normal, false);
    }

    void Textbox::onMouseLeave() {
        setState(WidgetState_Hovered, false);
        setState(WidgetState_Normal, true);
    }

    size_t Textbox::getIndexOfPreviousWord() {
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

    size_t Textbox::getIndexOfNextWord() {
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

    size_t Textbox::getIndexOfPreviousNewLineChar() {
        // Start searching from the cursorIndex - 1 to avoid counting the current position
        for (size_t i = cursorIndex; i > 0; --i) {
            if (text[i - 1] == '\n') {
                return i; // Return the index of the newline character
            }
        }
        return 0; // No previous newline found
    }

    size_t Textbox::getIndexOfNextNewLineChar() {
        // Start searching from the cursorIndex to the end of the string
        for (size_t i = cursorIndex; i < text.size(); ++i) {
            if (text[i] == '\n') {
                return i; // Return the index of the newline character
            }
        }
        return text.size(); // No next newline found
    }

    bool Textbox::isSpaceOrPunct(char c) const {
        return std::isspace(c) || std::ispunct(c);
    }

    Vector2 Textbox::calculateCursorPosition() {
        Vector2 position = getPosition();
        Vector2 size = getSize();
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

    Vector2 Textbox::calculateTextPosition() {
        Vector2 position = getPosition();
        Vector2 size = getSize();
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
}
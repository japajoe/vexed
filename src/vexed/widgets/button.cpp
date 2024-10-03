#include "button.h"

namespace vexed {
    Button::Button() : Widget(), IFont() {
        setPosition(Vector2(0, 0));
        setSize(Vector2(100, 20));
        setText("Button");
        setFontSize(16);
    }

    std::string Button::getText() const {
        return text;
    }

    void Button::setText(const std::string &text) {
        this->text = text;
    }

    void Button::onRender() {
        auto state = getState();
        auto position = getPosition();
        auto size = getSize();
        
        Rectangle clippingRect(position.x, position.y, size.x, size.y);
        Color buttonColor = getColor(WidgetColor_ButtonNormal);
        Color textColor = getColor(WidgetColor_ButtonTextNormal);
        
        if(state & WidgetState_Hovered) {
            if(state & WidgetState_Focused) {
                buttonColor = getColor(WidgetColor_ButtonFocused);
                textColor = getColor(WidgetColor_ButtonTextFocused);
            } else {
                buttonColor = getColor(WidgetColor_ButtonHovered);
                textColor = getColor(WidgetColor_ButtonTextHovered);
            }
        }

        addRectangle(position, size, 0, buttonColor, Rectangle(0, 0, 0, 0), getShader());
        //addBorder(position, size, 1, Color(1, 1, 1, 0.3), BorderOptions_All);

        Vector2 textSize = getTextSize(text, getFontSize());
        Vector2 textPosition(position.x + (size.x - textSize.x) / 2.0f,
                             position.y + (size.y - textSize.y) / 2.0f);

        addText(textPosition, font, true, text, getFontSize(), textColor, clippingRect);
    }

    void Button::onButtonDown(ButtonCode buttoncode) {
        auto state = getState();

        if(state & WidgetState_Hovered) {
            if(!isAnyFocused()) {
                setState(WidgetState_Pressed, true);
                setState(WidgetState_Focused, true);
                setFocusedWidget(this);                
            }
        } else {
            if(isFocused()) {
                setState(WidgetState_Pressed, false);
                setState(WidgetState_Focused, false);
                setFocusedWidget(nullptr);
            }
        }
    }

    void Button::onButtonUp(ButtonCode buttoncode) {
        setState(WidgetState_Pressed, false);
        setState(WidgetState_Focused, false);

        if(isFocused()) {
            setFocusedWidget(nullptr);
            auto state = getState();
            if(state & WidgetState_Hovered) {
                if(click) {
                    click(this);
                }
            }
        }
    }

    void Button::onMouseEnter() {
        //Only set state if nothing is focused or this widget is the active focused
        if(!isAnyFocused() || isFocused()) {
            setState(WidgetState_Hovered, true);
            setState(WidgetState_Normal, false);
        }
    }

    void Button::onMouseLeave() {
        setState(WidgetState_Hovered, false);
        setState(WidgetState_Normal, true);
    }
}
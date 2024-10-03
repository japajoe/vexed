#include "arrowbutton.h"

namespace vexed {
    ArrowButton::ArrowButton() : Widget(), IFont() {
        setPosition(Vector2(0, 0));
        setSize(Vector2(20, 20));
        setFontSize(16);
        setDirection(ArrowDirection::Up);
    }

    ArrowDirection ArrowButton::getDirection() const {
        return direction;
    }

    void ArrowButton::setDirection(ArrowDirection direction) {
        this->direction = direction;
    }

    void ArrowButton::onRender() {
        auto state = getState();
        auto position = getPosition();
        auto size = getSize();
        
        Rectangle clippingRect(position.x, position.y, size.x, size.y);
        Color buttonColor = getColor(WidgetColor_ArrowButtonNormal);
        
        if(state & WidgetState_Hovered) {
            if(state & WidgetState_Focused) {
                buttonColor = getColor(WidgetColor_ArrowButtonFocused);
            } else {
                buttonColor = getColor(WidgetColor_ArrowButtonHovered);
            }
        }

        addRectangle(position, size, 0, buttonColor);

        //addBorder(position, size, 1, Color(1, 1, 1, 0.3), BorderOptions_All);

        float rotation = 0.0f;

        switch(direction) {
            case ArrowDirection::Up:
                rotation = 180.0f;
                break;
            case ArrowDirection::Down:
                rotation = 0.0f;
                break;
            case ArrowDirection::Left:
                rotation = -90.0f;
                break;
            case ArrowDirection::Right:
                rotation = 90.0f;
                break;
        }

        Vector2 arrowSize(size.x * 0.5, size.y * 0.5);

        Vector2 arrowPosition(position.x + (size.x * 0.5), position.y + (size.y * 0.5));
        addTriangle(arrowPosition, arrowSize, rotation, getColor(WidgetColor_ArrowButtonArrow));


    }

    void ArrowButton::onButtonDown(ButtonCode buttoncode) {
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

    void ArrowButton::onButtonUp(ButtonCode buttoncode) {
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

    void ArrowButton::onButtonRepeat(ButtonCode buttoncode) {
        if(isFocused()) {
            auto state = getState();
            if(state & WidgetState_Hovered) {
                if(click) {
                    click(this);
                }
            }
        }
    }

    void ArrowButton::onMouseEnter() {
        //Only set state if nothing is focused or this widget is the active focused
        if(!isAnyFocused() || isFocused()) {
            setState(WidgetState_Hovered, true);
            setState(WidgetState_Normal, false);
        }
    }

    void ArrowButton::onMouseLeave() {
        setState(WidgetState_Hovered, false);
        setState(WidgetState_Normal, true);
    }
}
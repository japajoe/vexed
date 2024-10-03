#include "sliderfloat.h"

namespace vexed {
    SliderFloat::SliderFloat() : Widget() {
        setPosition(Vector2(0, 0));
        setSize(Vector2(100, 16));
        setMinValue(0.0f);
        setMaxValue(1.0f);
        setValue(0.0f);
    }

    float SliderFloat::getValue() const {
        return value;
    }

    void SliderFloat::setValue(float value) {
        if(value < valueMin)
            value = valueMin;
        if(value > valueMax)
            value = valueMax;
        this->value = value;
    }

    float SliderFloat::getMinValue() const {
        return valueMin;
    }

    void SliderFloat::setMinValue(float value) {
        this->valueMin = value;
    }

    float SliderFloat::getMaxValue() const {
        return valueMax;
    }

    void SliderFloat::setMaxValue(float value) {
        this->valueMax = value;
    }

    void SliderFloat::onRender() {
        auto state = getState();
        auto position = getPosition();
        auto size = getSize();
        auto grabRect = getGrabRectangle();
        Color backgroundColor = getColor(WidgetColor_SliderNormal);
        Color grabColor = getColor(WidgetColor_SliderGrabNormal);
        
        if(state & WidgetState_Hovered) {
            if(state & WidgetState_Focused) {
                backgroundColor = getColor(WidgetColor_SliderFocused);
                grabColor = getColor(WidgetColor_SliderGrabFocused);
            } else {
                backgroundColor = getColor(WidgetColor_SliderHovered);
                grabColor = getColor(WidgetColor_SliderGrabHovered);
            }
        } else {
            if(state & WidgetState_Focused) {
                backgroundColor = getColor(WidgetColor_SliderFocused);
                grabColor = getColor(WidgetColor_SliderGrabFocused);
            }
        }

        addRectangle(position, size, 0, backgroundColor);
        addRectangle(Vector2(grabRect.x, grabRect.y), Vector2(grabRect.width, grabRect.height), 0, grabColor);

        if(isFocused()) {
            Mouse *mouse = Application::getInstance()->getMouse();
            float normalizedMouseX = (mouse->getX() - position.x) / size.x;
            float sliderValue = valueMin + normalizedMouseX * (valueMax - valueMin);
            setValue(sliderValue);
        }
    }

    void SliderFloat::onButtonDown(ButtonCode buttoncode) {
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

    void SliderFloat::onButtonUp(ButtonCode buttoncode) {
        setState(WidgetState_Pressed, false);
        setState(WidgetState_Focused, false);

        if(isFocused()) {
            setFocusedWidget(nullptr);
            // auto state = getState();
            // if(state & WidgetState_Hovered)
            //     printf("Clicked %zu\n", getId());
        }
    }

    void SliderFloat::onMouseEnter() {
        //Only set state if nothing is focused or this widget is the active focused
        if(!isAnyFocused() || isFocused()) {
            setState(WidgetState_Hovered, true);
            setState(WidgetState_Normal, false);
        }
    }

    void SliderFloat::onMouseLeave() {
        setState(WidgetState_Hovered, false);
        setState(WidgetState_Normal, true);
    }

    Rectangle SliderFloat::getGrabRectangle() {
        Vector2 position = getPosition();
        Vector2 size = getSize();
        Rectangle rect(position.x, position.y, size.x, size.y);
        float radius = size.y * 0.5f;
        float width = rect.width - (radius * 2.0f);
        float t = (value - valueMin) / (valueMax - valueMin);
        float x = rect.x + (t * width);
        float y = (rect.y + rect.height * 0.5f) - radius;
        return Rectangle(x, y, 2 * radius, 2 * radius);
    }
}
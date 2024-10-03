#include "widget.h"
#include "../vexed.h"

namespace vexed {
    Widget *Widget::focusedWidget = nullptr;

    static void setColors(Color *colors) {
        colors[WidgetColor_ArrowButtonNormal] = Color(50, 50, 50, 255);
        colors[WidgetColor_ArrowButtonHovered] = Color(70, 70, 70, 255);
        colors[WidgetColor_ArrowButtonFocused] = Color(90, 90, 90, 255);
        colors[WidgetColor_ArrowButtonArrow] = Color::white();

        colors[WidgetColor_ButtonNormal] = Color(50, 50, 50, 255);
        colors[WidgetColor_ButtonHovered] = Color(70, 70, 70, 255);
        colors[WidgetColor_ButtonFocused] = Color(90, 90, 90, 255);
        colors[WidgetColor_ButtonTextNormal] = Color(220, 220, 220, 255);
        colors[WidgetColor_ButtonTextHovered] = Color(235, 183, 52, 255);
        colors[WidgetColor_ButtonTextFocused] = Color(227, 168, 18, 255);

        colors[WidgetColor_ComboboxNormal] = Color(0.2f, 0.2f, 0.2f, 1.0f);
        colors[WidgetColor_ComboboxHovered] = Color(0.4f, 0.4f, 0.4f, 1.0f);
        colors[WidgetColor_ComboboxFocused] = Color(0.6f, 0.6f, 0.6f, 1.0f);
        colors[WidgetColor_ComboboxArrowBoxNormal] = Color(0.2f, 0.2f, 0.2f, 1.0f);
        colors[WidgetColor_ComboboxArrowBoxHovered] = Color(0.4f, 0.4f, 0.4f, 1.0f);
        colors[WidgetColor_ComboboxArrowBoxFocused] = Color(0.6f, 0.6f, 0.6f, 1.0f);
        colors[WidgetColor_ComboboxArrow] = Color::white();
        colors[WidgetColor_ComboboxRow] = Color(0.172f, 0.576f, 0.67f, 0.5f);
        colors[WidgetColor_ComboboxRowSelected] = Color(0.172f, 0.576f, 0.67f, 1.0f);

        colors[WidgetColor_ContainerBackground] = Color::transparent();

        //WidgetColor_LabelNormal
        colors[WidgetColor_LabelNormal] = Color::white();
        
        colors[WidgetColor_SliderNormal] = Color(50, 50, 50, 255);
        colors[WidgetColor_SliderHovered] = Color(70, 70, 70, 255);
        colors[WidgetColor_SliderFocused] = Color(90, 90, 90, 255);
        colors[WidgetColor_SliderGrabNormal] = Color(69, 181, 222, 255);
        colors[WidgetColor_SliderGrabHovered] = Color(69, 181, 222, 255); 
        colors[WidgetColor_SliderGrabFocused] = Color(69, 181, 222, 255); 


        colors[WidgetColor_TextboxNormal] = Color(50, 50, 50, 255);
        colors[WidgetColor_TextboxHovered] = Color(70, 70, 70, 255);
        colors[WidgetColor_TextboxFocused] = Color(90, 90, 90, 255);
        colors[WidgetColor_TextboxText] = Color(220, 220, 220, 255);
    }
    
    Widget::Widget() {
        id = reinterpret_cast<uint64_t>(this);
        state = WidgetState_Normal;
        parent = nullptr;
        shaderId = 0;
        setColors(colors);
    }

    Widget::~Widget() {

    }

    void Widget::setPosition(const Vector2 &position) {
        transform.setPosition(position);
    }

    void Widget::setSize(const Vector2 &size) {
        transform.setScale(size);
    }

    void Widget::setColor(WidgetColor colorIdx, const Color &color) {
        if(colorIdx < WidgetColor_COUNT)
            colors[colorIdx] = color;
    }

    Rectangle Widget::getClippingRectangle() const {
        auto position = getPosition();
        auto size = getSize();
        Rectangle rect(position.x, position.y, size.x, size.y);
        return rect;
    }

    Widget *Widget::getParent() const {
        return parent;
    }

    uint32_t Widget::getShader() const {
        return shaderId;
    }

    void Widget::setShader(uint32_t shaderId) {
        this->shaderId = shaderId;
    }

    void Widget::remove(const Widget *widget) {
        bool found = false;
        size_t index = 0;

        for(size_t i = 0; i < children.size(); i++) {
            if(children[i].get() == widget) {
                index = i;
                found = true;
                break;
            }
        }

        if(found) {
            children.erase(children.begin() + index);
        }
    }

    void Widget::setState(WidgetState s, bool enabled) {
        if(enabled)
            this->state |= s;
        else 
            this->state &= ~s;
    }

    bool Widget::isAnyFocused() const {
        return focusedWidget != nullptr;
    }

    bool Widget::isFocused() const {
        if(focusedWidget == nullptr)
            return false;
        return focusedWidget == this;
    }

    Widget *Widget::getFocusedWidget() const {
        return focusedWidget;
    }

    void Widget::setFocusedWidget(Widget *widget) {
        focusedWidget = widget;
    }

    void Widget::addRectangle(const Vector2 &position, const Vector2 &size, float rotationDegrees, const Color &color, const Rectangle &clippingRect, uint32_t shaderId) {
        auto graphics = Application::getInstance()->getGraphics();
        graphics->addRectangle(position, size, rotationDegrees, color, clippingRect, shaderId, this);
    }

    void Widget::addTriangle(const Vector2 &position, const Vector2 &size, float rotationDegrees, const Color &color, const Rectangle &clippingRect, uint32_t shaderId) {
        auto graphics = Application::getInstance()->getGraphics();
        graphics->addTriangle(position, size, rotationDegrees, color, clippingRect, shaderId, this);
    }

    void Widget::addText(const Vector2 &position, Font *font, bool richText, const std::string &text, float fontSize, const Color &color, const Rectangle &clippingRect) {
        auto graphics = Application::getInstance()->getGraphics();
        graphics->addText(position, font, richText, text, fontSize, color, clippingRect);
    }

    void Widget::addLines(Vector2 *segments, size_t count, float thickness, const Color &color, const Rectangle &clippingRect) {
        auto graphics = Application::getInstance()->getGraphics();
        graphics->addLines(segments, count, thickness, color, clippingRect, shaderId, this);
    }

    void Widget::addBorder(const Vector2 &position, const Vector2 &size, float thickness, const Color &color, BorderOptions borderOptions, const Rectangle &clippingRect) {
        Rectangle outerRect(position.x, position.y, size.x, size.y);
        float innerOffset = 0.0f;

        Vector2 innerTopLeft(outerRect.x + innerOffset, outerRect.y + innerOffset);
        Vector2 innerTopRight(outerRect.x + outerRect.width - innerOffset, outerRect.y + innerOffset);
        Vector2 innerBottomLeft(outerRect.x + innerOffset, outerRect.y + outerRect.height - innerOffset);
        Vector2 innerBottomRight(outerRect.x + outerRect.width - innerOffset, outerRect.y + outerRect.height - innerOffset);

        // Fixed size for maximum lines (up to 8 for 4 borders)
        Vector2 lines[8];
        int lineCount = 0;

        // Check each border option and add the corresponding lines
        if (borderOptions & BorderOptions_Top) {
            lines[lineCount++] = innerTopLeft;
            lines[lineCount++] = innerTopRight;
        }
        if (borderOptions & BorderOptions_Right) {
            lines[lineCount++] = innerTopRight;
            lines[lineCount++] = innerBottomRight;
        }
        if (borderOptions & BorderOptions_Bottom) {
            lines[lineCount++] = innerBottomRight;
            lines[lineCount++] = innerBottomLeft;
        }
        if (borderOptions & BorderOptions_Left) {
            lines[lineCount++] = innerBottomLeft;
            lines[lineCount++] = innerTopLeft;
        }

        //addLines(lines, 4, thickness, color, zOrder);
        addLines(lines, lineCount / 2, thickness, color, clippingRect);
    }

    bool Widget::containsPoint(const Vector2 &point) {
        auto position = getPosition();
        auto size = getSize();
        return (point.x >= position.x && point.x <= position.x + size.x &&
                point.y >= position.y && point.y <= position.y + size.y);
    }
}
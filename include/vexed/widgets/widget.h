#ifndef VEXED_WIDGET_H
#define VEXED_WIDGET_H

#include "../vexed.h"
#include "transform2d.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <type_traits>

namespace vexed {
    enum WidgetState_ {
        WidgetState_Normal = 1 << 0,
        WidgetState_Hovered = 1 << 1,
        WidgetState_Focused = 1 << 2,
        WidgetState_Pressed = 1 << 3
    };

    typedef int WidgetState;

    enum WidgetColor_ {
        WidgetColor_ArrowButtonNormal,
        WidgetColor_ArrowButtonHovered,
        WidgetColor_ArrowButtonFocused,
        WidgetColor_ArrowButtonArrow,
        WidgetColor_ButtonNormal,
        WidgetColor_ButtonHovered,
        WidgetColor_ButtonFocused,
        WidgetColor_ButtonTextNormal,
        WidgetColor_ButtonTextHovered,
        WidgetColor_ButtonTextFocused,
        WidgetColor_ComboboxNormal,
        WidgetColor_ComboboxHovered,
        WidgetColor_ComboboxFocused,
        WidgetColor_ComboboxArrowBoxNormal,
        WidgetColor_ComboboxArrowBoxHovered,
        WidgetColor_ComboboxArrowBoxFocused,
        WidgetColor_ComboboxArrow,
        WidgetColor_ComboboxRow,
        WidgetColor_ComboboxRowSelected,
        WidgetColor_ContainerBackground,
        WidgetColor_LabelNormal,
        WidgetColor_SliderNormal,
        WidgetColor_SliderHovered,
        WidgetColor_SliderFocused,
        WidgetColor_SliderGrabNormal,
        WidgetColor_SliderGrabHovered,
        WidgetColor_SliderGrabFocused,
        WidgetColor_TextboxNormal,
        WidgetColor_TextboxHovered,
        WidgetColor_TextboxFocused,
        WidgetColor_TextboxText,
        WidgetColor_COUNT
    };

    typedef int WidgetColor;

    enum BorderOptions_ {
        BorderOptions_Left = 1 << 0,
        BorderOptions_Right = 1 << 1,
        BorderOptions_Top = 1 << 2,
        BorderOptions_Bottom = 1 << 3,
        BorderOptions_All = BorderOptions_Left | BorderOptions_Right | BorderOptions_Top | BorderOptions_Bottom
    };

    typedef int BorderOptions;

    class Widget;
    using ClickCallback = std::function<void(const Widget *sender)>;
    using SubmitCallback = std::function<void(const Widget *sender)>;

    class Widget {
    friend class Canvas;
    public:
        Widget();
        virtual ~Widget();
        uint64_t getId() const { return id; }
        Vector2 getPosition() const { return transform.getWorldPosition(); }
        void setPosition(const Vector2 &position);
        Vector2 getSize() const { return transform.getScale(); }
        void setSize(const Vector2 &size);
        Color getColor(WidgetColor colorIdx) const { return colors[colorIdx]; }
        void setColor(WidgetColor colorIdx, const Color &color);
        Rectangle getClippingRectangle() const;
        Widget *getParent() const;
        uint32_t getShader() const;
        void setShader(uint32_t shaderId);
        void remove(const Widget *widget);

        template <typename T, typename... Param>
        T* add(Param... param) {
            static_assert(std::is_base_of<Widget, T>::value, "add parameter must derive from Widget");

            std::unique_ptr<Widget> ptr = std::make_unique<T>(param...);
            if (!ptr)
                return nullptr;

            children.push_back(std::move(ptr));
            size_t last = children.size() - 1;
            Widget *widget = children[last].get();
            widget->transform.setParent(&transform);
            widget->parent = this;

            return static_cast<T*>(widget);
        }
        WidgetState getState() const { return state; }
    protected:
        void setState(WidgetState s, bool enabled);
        bool isAnyFocused() const;
        bool isFocused() const;
        Widget *getFocusedWidget() const;
        void setFocusedWidget(Widget *widget);
        void addRectangle(const Vector2 &position, const Vector2 &size, float rotationDegrees, const Color &color, const Rectangle &clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0);
        void addTriangle(const Vector2 &position, const Vector2 &size, float rotationDegrees, const Color &color, const Rectangle &clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0);
        void addText(const Vector2 &position, Font *font, bool richText, const std::string &text, float fontSize, const Color &color, const Rectangle &clippingRect = Rectangle(0, 0, 0, 0));
        void addLines(Vector2 *segments, size_t count, float thickness, const Color &color, const Rectangle &clippingRect = Rectangle(0, 0, 0, 0));
        void addBorder(const Vector2 &position, const Vector2 &size, float thickness, const Color &color, BorderOptions borderOptions, const Rectangle &clippingRect = Rectangle(0, 0, 0, 0));
        virtual bool containsPoint(const Vector2 &point);
        virtual void onRender() {}
        virtual void onCharPress(uint32_t codepoint) {}
        virtual void onKeyDown(KeyCode keycode) {}
        virtual void onKeyUp(KeyCode keycode) {}
        virtual void onKeyPress(KeyCode keycode) {}
        virtual void onKeyRepeat(KeyCode keycode) {}
        virtual void onButtonDown(ButtonCode buttoncode) {}
        virtual void onButtonUp(ButtonCode buttoncode) {}
        virtual void onButtonPress(ButtonCode buttoncode) {}
        virtual void onButtonRepeat(ButtonCode buttoncode) {}
        virtual void onMouseEnter() {}
        virtual void onMouseLeave() {}
    private:
        Transform2D transform;
        uint64_t id;
        WidgetState state;
        Widget *parent;
        std::vector<std::unique_ptr<Widget>> children;
        Color colors[WidgetColor_COUNT];
        uint32_t shaderId;
        static Widget *focusedWidget;
    };
}

#endif
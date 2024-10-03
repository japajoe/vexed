#include "combobox.h"

namespace vexed {
    Combobox::Combobox() : Widget(), IFont() {
        setPosition(Vector2(0, 0));
        setSize(Vector2(100, 20));
        setFontSize(16);

        selectedIndex = -1;
        showItems = false;

        addItem("Chicken and gravy");
        addItem("Watermelon");
        addItem("Macaroni and cheese");
        addItem("Corn on the cob");
        addItem("Grape drink");
        addItem("Apple drink");
        addItem("Grape juice");
        addItem("Apple juice");
    }

    void Combobox::addItem(const std::string &text) {
        items.push_back(text);
    }

    void Combobox::setItem(const std::string &text, size_t index) {
        if(index < items.size() && items.size() > 0)
            items[index] = text;
    }

    void Combobox::removeItem(size_t index) {
        if(index < items.size() && items.size() > 0)
            items.erase(items.begin() + index);
    }

    void Combobox::setSelectedIndex(uint32_t index) {
        if(index < items.size() && items.size() > 0)
            selectedIndex = index;
    }

    void Combobox::onRender() {
        float arrowSize = 10;
        float paddingX = 4;
        float paddingY = 2;

        Vector2 pos = getPosition();
        Vector2 size = getSize();
        Vector2 positionBoxMain = getPosition();
        Vector2 sizeBoxMain(size.x - (arrowSize * 2), size.y);

        Vector2 positionBoxRight(positionBoxMain.x + sizeBoxMain.x, positionBoxMain.y);
        Vector2 sizeBoxRight(arrowSize * 2, size.y);

        Vector2 sizeArrow(arrowSize, arrowSize);
        Vector2 positionArrow(positionBoxRight.x + arrowSize, positionBoxMain.y + sizeArrow.y);

        Vector2 textPosition(pos.x + paddingX, pos.y + paddingY);

        Color colorArrowBox = getColor(WidgetColor_ComboboxNormal);
        
        auto state = getState();

        if(state & WidgetState_Hovered) {
            if(state & WidgetState_Focused) {
                colorArrowBox = getColor(WidgetColor_ComboboxFocused);
            } else {
                colorArrowBox = getColor(WidgetColor_ComboboxHovered);
            }
        }

        addRectangle(positionBoxMain, sizeBoxMain, 0, getColor(WidgetColor_ComboboxNormal));
        addRectangle(positionBoxRight, sizeBoxRight, 0, colorArrowBox);
        
        //BorderOptions borderOptions =  BorderOptions_All;
        //addBorder(pos, size, 1, getColor(WidgetColor_ButtonFocused), borderOptions);
        
        addTriangle(positionArrow, sizeArrow, 0, getColor(WidgetColor_ComboboxArrow));

        Rectangle clippingRect(positionBoxMain.x, positionBoxMain.y, sizeBoxMain.x, sizeBoxMain.y);

        if(selectedIndex >= 0) {
            addText(textPosition, font, true, items[selectedIndex], fontSize, Color::white(), clippingRect);
        } else {
            addText(textPosition, font, true, "Select option...", fontSize, Color::white(), clippingRect);
        }

        if(showItems && items.size() > 0) {
            Vector2 positionOptions(pos.x, pos.y + size.y);
            //Vector2 sizeOptions(0, 8);
            Vector2 sizeOptions(0, 4);

            for(size_t i = 0; i < items.size(); i++) {
                float w = font->computeTextWidth(items[i], fontSize) + 8;
                float h = font->getLineHeight() * (fontSize / font->getPixelSize());
                if(w > sizeOptions.x)
                    sizeOptions.x = w;
                sizeOptions.y += h;
            }

            if(sizeOptions.x < size.x)
                sizeOptions.x = size.x;

            //addRectangle(positionOptions, sizeOptions, getColor(WidgetColor_ButtonNormal), 3);
            addRectangle(positionOptions, sizeOptions, 0, Color(0.1f, 0.1f, 0.1f, 1.0f));


            //borderOptions =   BorderOptions_Left | BorderOptions_Right | BorderOptions_Bottom;

            //borderOptions =   BorderOptions_All;

            //addBorder(positionOptions, sizeOptions, 1, getColor(WidgetColor_ButtonFocused), borderOptions);


            clippingRect = Rectangle(positionOptions.x, positionOptions.y, sizeOptions.x, sizeOptions.y);

            Mouse *mouse = Application::getInstance()->getMouse();
            float mx = mouse->getX();
            float my = mouse->getY();

            float h = clippingRect.height / items.size();
            Vector2 rowPos(clippingRect.x, clippingRect.y + selectedIndex * h);
            Vector2 rowSize(clippingRect.width, h);

            if(selectedIndex >= 0)
                addRectangle(rowPos, rowSize, 0, getColor(WidgetColor_ComboboxRow));
            
            auto rectContains = [](const Rectangle &rectangle, const Vector2 &mousePosition) -> bool {
                return (mousePosition.x >= rectangle.x && mousePosition.x <= rectangle.x + rectangle.width &&
                        mousePosition.y >= rectangle.y && mousePosition.y <= rectangle.y + rectangle.height);
            };

            if(rectContains(clippingRect, Vector2(mx, my))) {
                float relativeY = my - clippingRect.y;
                int rowIndex = static_cast<int>(relativeY / h);

                if(rowIndex < items.size()) {
                    rowPos = Vector2(clippingRect.x, clippingRect.y + rowIndex * h);
                    rowSize = Vector2(clippingRect.width, h);

                    addRectangle(rowPos, rowSize, 0, getColor(WidgetColor_ComboboxRowSelected));

                    if(mouse->getButtonDown(ButtonCode::Left)) {
                        setSelectedIndex(rowIndex);
                    }
                }
            }

            std::string text;
            for(size_t i = 0; i < items.size(); i++) {
                text += items[i] + "\n";
            }

            addText(Vector2(positionOptions.x + 4, positionOptions.y), font, true, text, fontSize, Color::white(), clippingRect);
        }
    }

    void Combobox::onButtonDown(ButtonCode buttoncode) {
        auto state = getState();

        if(state & WidgetState_Hovered) {
            if(!isAnyFocused()) {
                setState(WidgetState_Pressed, true);
                setState(WidgetState_Focused, true);
                setFocusedWidget(this);
                showItems = !showItems;
            }
        } else {
            if(isFocused()) {
                setState(WidgetState_Pressed, false);
                setState(WidgetState_Focused, false);
                setFocusedWidget(nullptr);
            }
        }
    }

    void Combobox::onButtonUp(ButtonCode buttoncode) {
        setState(WidgetState_Pressed, false);
        setState(WidgetState_Focused, false);

        if(isFocused()) {
            setFocusedWidget(nullptr);
            // auto state = getState();
            // if(state & WidgetState_Hovered)
            //     printf("Clicked %zu\n", getId());
        } else {
            showItems = false;
        }
    }

    void Combobox::onMouseEnter() {
        //Only set state if nothing is focused or this widget is the active focused
        if(!isAnyFocused() || isFocused()) {
            setState(WidgetState_Hovered, true);
            setState(WidgetState_Normal, false);
        }
    }

    void Combobox::onMouseLeave() {
        setState(WidgetState_Hovered, false);
        setState(WidgetState_Normal, true);
    }
}
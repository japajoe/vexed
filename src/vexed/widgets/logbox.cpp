#include "logbox.h"

namespace vexed {
    Logbox::Logbox() : Widget(), IFont() {
        const size_t lineHeight = font->getLineHeight() * (fontSize / font->getPixelSize());
        setPosition(Vector2(0, 0));
        setFontSize(16);
        setSize(Vector2(400, 10 * lineHeight));
        scrollPosition = 0;
        textOffset = Vector2(0, 4);
    }

    void Logbox::addMessage(const std::string &message) {
        messages.add(message);

        // const size_t lineHeight = font->getLineHeight() * (fontSize / font->getPixelSize());
        // if(messages.count() * lineHeight >= getSize().y)
        //     scrollDown();
    }

    void Logbox::onRender() {
        if(messages.count() == 0)
            return;

        const size_t widthVertical = 12;
        const size_t lineHeight = font->getLineHeight() * (fontSize / font->getPixelSize()) ;
        Vector2 position = getPosition();
        Vector2 size = getSize();
        Rectangle clippingRect(position.x, position.y, size.x - widthVertical, size.y - textOffset.y);

        visibleMessagesCount = size.y / lineHeight;
        totalMessagesCount = messages.count();

        // Start rendering messages from scrollPosition
        for(size_t i = scrollPosition; i < scrollPosition + visibleMessagesCount && i < totalMessagesCount; i++) {
            auto &message = messages.getAt(i);
            float textHeight = font->computeTextHeight(message, fontSize);
            Vector2 textPosition(position.x + textOffset.x, position.y + (textHeight * (i - scrollPosition)) + textOffset.y);
            addText(textPosition, font, true, message, fontSize, Color::white(), clippingRect);
        }
        
        Vector2 positionVertical(position.x + size.x - widthVertical, position.y);
        Vector2 sizeVertical(widthVertical, size.y);
        addRectangle(positionVertical, sizeVertical, 0, Color(1, 1, 1, 0.1));

        // Calculate the scrollbar height based on visible messages
        float scrollbarHeight = (static_cast<float>(visibleMessagesCount) / totalMessagesCount) * size.y;

        if (scrollbarHeight < 10) {
            scrollbarHeight = 10;
        }

        if (scrollbarHeight > size.y) {
            scrollbarHeight = size.y; 
        }

        float scrollbarYPosition = position.y + (scrollPosition / static_cast<float>(totalMessagesCount - visibleMessagesCount)) * (size.y - scrollbarHeight);

        if(scrollbarYPosition != scrollbarYPosition) { //NaN
            scrollbarYPosition = position.y;
        }

        // Draw the scrollbar indicator
        Vector2 positionGrab(positionVertical.x, scrollbarYPosition);
        Vector2 sizeGrab(widthVertical, scrollbarHeight);
        
        scrollbarRect = Rectangle(positionGrab.x, positionGrab.y, widthVertical, scrollbarHeight);

        addRectangle(positionGrab, sizeGrab, 0, Color::skyBlue());
    }

    void Logbox::onButtonPress(ButtonCode buttoncode) {
        if(isAnyFocused())
            return;

        Vector2 position = getPosition();
        Vector2 size = getSize();
        const size_t widthVertical = 12;
        Vector2 positionVertical(position.x + size.x - widthVertical, position.y);

        auto isMouseOverScrollbar = [] (const Rectangle& scrollbarRect) -> bool {
            auto mouse = Application::getInstance()->getMouse();
            float mouseX = mouse->getX();
            float mouseY = mouse->getY();
            return mouseX >= scrollbarRect.x && mouseX <= scrollbarRect.x + scrollbarRect.width &&
                mouseY >= scrollbarRect.y && mouseY <= scrollbarRect.y + scrollbarRect.height;
        };

        auto mouse = Application::getInstance()->getMouse();

        // Check if the mouse is within the scrollbar area
        if (isMouseOverScrollbar(scrollbarRect)) {
            isDraggingScrollbar = true;
            initialMouseY = mouse->getY(); // Capture the current mouse Y position
            scrollStartPosition = scrollPosition; // Capture the current scroll position
        }

        if (isDraggingScrollbar) {
            // Calculate how far the mouse has moved
            float deltaY = mouse->getY() - initialMouseY;

            // Calculate new scroll position based on delta movement
            float totalHeight = getSize().y; // Total height of the log box
            float scrollbarHeight = (static_cast<float>(visibleMessagesCount) / totalMessagesCount) * totalHeight;

            // Calculate the new scroll position
            float newScrollPosition = scrollStartPosition + (deltaY / (totalHeight - scrollbarHeight)) * (totalMessagesCount - visibleMessagesCount);
            
            // Clamp the scroll position to valid range
            if (newScrollPosition < 0) {
                newScrollPosition = 0;
            } else if (newScrollPosition > totalMessagesCount - visibleMessagesCount) {
                newScrollPosition = totalMessagesCount - visibleMessagesCount;
            }

            scrollPosition = static_cast<size_t>(newScrollPosition);
        }
    }

    void Logbox::onButtonUp(ButtonCode buttoncode) {
        isDraggingScrollbar = false;
    }

    void Logbox::scrollUp() {
        if (scrollPosition > 0) {
            scrollPosition--;
        }
    }

    void Logbox::scrollDown() {
        if (scrollPosition < totalMessagesCount - visibleMessagesCount) {
            scrollPosition++;
        }
    }
}
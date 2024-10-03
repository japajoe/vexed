#ifndef VEXED_LOGBOX_H
#define VEXED_LOGBOX_H

#include "widget.h"
#include "interfaces/ifont.h"
#include "ringbuffer.h"

namespace vexed {
    class Logbox : public Widget, public IFont {
    public:
        Logbox();
        void addMessage(const std::string &message);
        void scrollUp();
        void scrollDown();
    protected:
        void onRender() override;
        void onButtonPress(ButtonCode buttoncode) override;
        void onButtonUp(ButtonCode buttoncode) override;
    private:
        RingBuffer<std::string> messages;
        size_t scrollPosition = 0;
        bool isDraggingScrollbar = false;
        float initialMouseY = 0.0f;
        size_t scrollStartPosition = 0;
        size_t visibleMessagesCount = 0;
        size_t totalMessagesCount = 0;
        Rectangle scrollbarRect;
        Vector2 textOffset;
    };
}

#endif
#ifndef VEXED_BUTTON_H
#define VEXED_BUTTON_H

#include "widget.h"
#include "interfaces/ifont.h"

namespace vexed {
    class Button : public Widget, public IFont {
    public:
        ClickCallback click;
        Button();
        std::string getText() const;
        void setText(const std::string &text);
    protected:
        void onRender() override;
        void onButtonDown(ButtonCode buttoncode) override;
        void onButtonUp(ButtonCode buttoncode) override;
        void onMouseEnter() override;
        void onMouseLeave() override;
    private:
        std::string text;        
    };
}

#endif
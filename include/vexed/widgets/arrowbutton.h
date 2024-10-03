#ifndef VEXED_ARROWBUTTON_H
#define VEXED_ARROWBUTTON_H

#include "widget.h"
#include "interfaces/ifont.h"

namespace vexed {
    enum class ArrowDirection {
        Up,
        Down,
        Left,
        Right
    };

    class ArrowButton : public Widget, public IFont {
    public:
        ClickCallback click;
        ArrowButton();
        ArrowDirection getDirection() const;
        void setDirection(ArrowDirection direction);
    protected:
        void onRender() override;
        void onButtonDown(ButtonCode buttoncode) override;
        void onButtonUp(ButtonCode buttoncode) override;
        void onButtonRepeat(ButtonCode buttoncode) override;
        void onMouseEnter() override;
        void onMouseLeave() override;
    private:
        ArrowDirection direction;
    };
}

#endif
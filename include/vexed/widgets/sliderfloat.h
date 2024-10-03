#ifndef VEXED_SLIDERFLOAT_H
#define VEXED_SLIDERFLOAT_H

#include "widget.h"

namespace vexed {
    class SliderFloat : public Widget {
    public:
        SliderFloat();
        float getValue() const;
        void setValue(float value);
        float getMinValue() const;
        void setMinValue(float value);
        float getMaxValue() const;
        void setMaxValue(float value);
    protected:
        void onRender() override;
        void onButtonDown(ButtonCode buttoncode) override;
        void onButtonUp(ButtonCode buttoncode) override;
        void onMouseEnter() override;
        void onMouseLeave() override;
    private:
        float value;
        float valueMin;
        float valueMax;
        Rectangle getGrabRectangle();
    };
}

#endif
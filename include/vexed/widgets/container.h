#ifndef VEXED_CONTAINER_H
#define VEXED_CONTAINER_H

#include "widget.h"

namespace vexed {
    class Container : public Widget {
    public:
        Container();
    protected:
        void onRender() override;
    };
}

#endif
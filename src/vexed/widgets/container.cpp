#include "container.h"

namespace vexed {
    Container::Container() : Widget() {
        setPosition(Vector2(0, 0));
        setSize(Vector2(256, 256));
    }

    void Container::onRender() {
        Color color = getColor(WidgetColor_ContainerBackground);
        if(color.a > 0) {
            Vector2 position = getPosition();
            Vector2 size = getSize();
            addRectangle(position, size, 0, color, Rectangle(0, 0, 0, 0), getShader());
        }
    }
}
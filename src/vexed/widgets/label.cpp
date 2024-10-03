#include "label.h"

namespace vexed {
    Label::Label() : Widget(), IFont() {
        setPosition(Vector2(0, 0));
        setSize(Vector2(100, 20));
        setText("Label");
        setFontSize(16);
    }

    std::string Label::getText() const {
        return text;
    }

    void Label::setText(const std::string &text) {
        this->text = text;
    }

    void Label::onRender() {
        auto position = getPosition();
        auto size = getSize();
        addText(position, font, true, text, getFontSize(), getColor(WidgetColor_LabelNormal));
    }
}
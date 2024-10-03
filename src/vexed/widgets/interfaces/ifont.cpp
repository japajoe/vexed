#include "ifont.h"

namespace vexed {
    IFont::IFont() {
        setFont(Font::find("Default"));
        setFontSize(20);
    }

    IFont::~IFont() {

    }
    
    void IFont::setFont(Font *font) {
        this->font = font;
    }

    void IFont::setFontSize(float size) {
        this->fontSize = size;
    }

    Vector2 IFont::getTextSize(const std::string &text, float fontSize) {
        float w = font->computeTextWidth(text, fontSize);
        float h = font->computeTextHeight(text, fontSize);
        return Vector2(w, h);
    }
}
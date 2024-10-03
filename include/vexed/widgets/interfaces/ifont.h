#ifndef VEXED_IFONT_H
#define VEXED_IFONT_H

#include "../../vexed.h"

namespace vexed {
    class IFont {
    public:
        IFont();
        virtual ~IFont();
        Font *getFont() const { return font; }
        void setFont(Font *font);
        void setFontSize(float size);
        float getFontSize() const { return fontSize; }
        Vector2 getTextSize(const std::string &text, float fontSize);
    protected:
        Font *font;
        float fontSize;
    };
}

#endif
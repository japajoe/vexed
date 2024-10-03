#ifndef VEXED_LABEL_H
#define VEXED_LABEL_H

#include "widget.h"
#include "interfaces/ifont.h"

namespace vexed {
    class Label : public Widget, public IFont {
    public:
        Label();
        std::string getText() const;
        void setText(const std::string &text);
    protected:
        void onRender() override;
    private:
        std::string text;        
    };
}

#endif
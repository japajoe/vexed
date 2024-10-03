#ifndef VEXED_COMBOBOX_H
#define VEXED_COMBOBOX_H

#include "widget.h"
#include "interfaces/ifont.h"
#include <vector>

namespace vexed {
    class Combobox : public Widget, public IFont {
    public:
        Combobox();
        void addItem(const std::string &text);
        void setItem(const std::string &text, size_t index);
        void removeItem(size_t index);
        int32_t getSelectedIndex() const { return selectedIndex; }
        void setSelectedIndex(uint32_t index);
    protected:
        void onRender() override;
        void onButtonDown(ButtonCode buttoncode) override;
        void onButtonUp(ButtonCode buttoncode) override;
        void onMouseEnter() override;
        void onMouseLeave() override;
    private:
        std::string title;
        std::vector<std::string> items;
        bool showItems;        
        int32_t selectedIndex;
    };
}

#endif
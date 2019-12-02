#pragma once
#include <pu/Plutonium>

using namespace pu::ui::elm;
namespace inst::ui {
    class OptionsPage : public pu::ui::Layout
    {
        public:
            OptionsPage();
            PU_SMART_CTOR(OptionsPage)
            void OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos) override;
            void OnStart() override;
            bool OnStop() override;
            void OnTick() override;
        private:
            TextBlock::Ref butText;
            Rectangle::Ref topRect;
            Rectangle::Ref infoRect;
            Rectangle::Ref botRect;
            Image::Ref titleImage;
            TextBlock::Ref appVersionText;
            TextBlock::Ref pageInfoText;
            pu::ui::elm::Menu::Ref menu;
            void setMenuText();
            std::string getMenuOptionIcon(bool ourBool);
    };
}
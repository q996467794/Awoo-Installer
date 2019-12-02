#pragma once
#include <pu/Plutonium>

using namespace pu::ui::elm;
namespace inst::ui {
    class MainPage : public pu::ui::Layout
    {
        public:
            MainPage();
            PU_SMART_CTOR(MainPage)
            void installMenuItem_Click();
            void netInstallMenuItem_Click();
            void usbInstallMenuItem_Click();
            void sigPatchesMenuItem_Click();
            void settingsMenuItem_Click();
            void exitMenuItem_Click();
            void OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos) override;
            void OnStart() override;
            bool OnStop() override;
            void OnTick() override;
            Image::Ref awooImage;
        private:
            TextBlock::Ref butText;
            Rectangle::Ref topRect;
            Rectangle::Ref botRect;
            Image::Ref titleImage;
            TextBlock::Ref appVersionText;
            Menu::Ref optionMenu;
            MenuItem::Ref installMenuItem;
            MenuItem::Ref netInstallMenuItem;
            MenuItem::Ref usbInstallMenuItem;
            MenuItem::Ref sigPatchesMenuItem;
            MenuItem::Ref settingsMenuItem;
            MenuItem::Ref exitMenuItem;
            Image::Ref eggImage;
    };
}
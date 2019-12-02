#pragma once
#include <pu/Plutonium>
#include <future>

using namespace pu::ui::elm;
namespace inst::ui {
    class netInstPage : public pu::ui::Layout
    {
        public:
            netInstPage();
            PU_SMART_CTOR(netInstPage)
            void OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos) override;
            void OnStart() override;
            bool OnStop() override;
            void OnTick() override;
        private:
            void startInstall(bool urlMode);
            std::vector<std::string> ourUrls;
            std::vector<std::string> selectedUrls;
            std::vector<std::string> alternativeNames;
            TextBlock::Ref butText;
            Rectangle::Ref topRect;
            Rectangle::Ref infoRect;
            Rectangle::Ref botRect;
            Image::Ref titleImage;
            TextBlock::Ref pageInfoText;
            TextBlock::Ref appVersionText;
            pu::ui::elm::Menu::Ref menu;
            Image::Ref infoImage;
            void drawMenuItems(bool clearItems);
            void selectTitle(int selectedIndex);
            bool waiting = false;
            std::future<std::vector<std::string>> future;
    };
}
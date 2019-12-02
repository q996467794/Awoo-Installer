#pragma once
#include <filesystem>
#include <pu/Plutonium>

using namespace pu::ui::elm;
namespace inst::ui {
    class SdInstPage : public pu::ui::Layout
    {
        public:
            SdInstPage();
            PU_SMART_CTOR(SdInstPage)
            void OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos) override;
            void OnStart() override;
            bool OnStop() override;
            void OnTick() override;
        private:
            void startInstall();
            TextBlock::Ref pageInfoText;
            void drawMenuItems(bool clearItems, std::filesystem::path ourPath);
            std::vector<std::filesystem::path> ourDirectories;
            std::vector<std::filesystem::path> ourFiles;
            std::vector<std::filesystem::path> selectedTitles;
            std::filesystem::path currentDir;
            pu::ui::elm::Menu::Ref menu;
            TextBlock::Ref butText;
            Rectangle::Ref topRect;
            Rectangle::Ref infoRect;
            Rectangle::Ref botRect;
            Image::Ref titleImage;
            TextBlock::Ref appVersionText;
            void followDirectory();
            void selectNsp(int selectedIndex);
    };
}
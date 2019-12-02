#pragma once
#include <pu/Plutonium>

using namespace std;
using namespace pu::ui::elm;
namespace inst::ui {
    class NetInstallPage : public pu::ui::Layout {
    public:
        NetInstallPage(unordered_map<string, string> links, string source);
        PU_SMART_CTOR(NetInstallPage)
        void OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos) override;
        void OnStart() override;
        bool OnStop() override;
        void OnTick() override;
    private:
        void OnSelect(MenuItem::Ref &item, pair<string, string> urlPair);
        void StartInstall();
        TextBlock::Ref butText;
        TextBlock::Ref pageInfoText;
        Rectangle::Ref topRect;
        Rectangle::Ref infoRect;
        Rectangle::Ref botRect;
        Image::Ref titleImage;
        TextBlock::Ref appVersionText;
        Image::Ref infoImage;
        Menu::Ref menu;
        unordered_map<string,string> m_links;
        vector<string> m_selected;
        string m_source;
    };
}

#pragma once
#include <pu/Plutonium>

using namespace std;
using namespace pu::ui::elm;
namespace inst::ui {
    class NetWaitPage : public pu::ui::Layout {
    public:
        NetWaitPage();
        PU_SMART_CTOR(NetWaitPage)
        void OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos) override;
        void OnStart() override;
        bool OnStop() override;
        void OnTick() override;
    private:
        void EnterURL();
        void EnterGdrive();
        TextBlock::Ref butText;
        TextBlock::Ref pageInfoText;
        Rectangle::Ref topRect;
        Rectangle::Ref infoRect;
        Rectangle::Ref botRect;
        Image::Ref titleImage;
        TextBlock::Ref appVersionText;
        Image::Ref infoImage;
        string lastUrl = "https://";
        string lastFileID = "";
    };
}

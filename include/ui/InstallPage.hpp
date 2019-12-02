#pragma once
#include <pu/Plutonium>

#include "install/install.hpp"
#include "functional"

using namespace std;
using namespace pu::ui::elm;
namespace inst::ui {
    class InstallPage : public pu::ui::Layout
    {
        public:
            InstallPage(unordered_map<string,string> installMap, NcmStorageId storageId, function<shared_ptr<tin::install::Install>(string,NcmStorageId)> GetTask, function<void()> OnFinished);
            PU_SMART_CTOR(InstallPage)
            void OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos) override;
            void OnStart() override;
            bool OnStop() override;
            void OnTick() override;
        private:
            void OnProgress(string title, double percentage);
            unordered_map<string,string> m_taskMap;
            NcmStorageId m_storageId;
            function<shared_ptr<tin::install::Install>(string,NcmStorageId)> m_cbTask;
            function<void()> m_cbFinish;
            TextBlock::Ref pageInfoText;
            TextBlock::Ref installInfoText;
            pu::ui::elm::ProgressBar::Ref installBar;
            Image::Ref awooImage;
            Rectangle::Ref infoRect;
            Rectangle::Ref topRect;
            Image::Ref titleImage;
            TextBlock::Ref appVersionText;
    };
}
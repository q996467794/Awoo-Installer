#include <filesystem>
#include "ui/MainApplication.hpp"
#include "ui/InstallPage.hpp"
#include "util/config.hpp"
#include "util/error.hpp"
#include "util/util.hpp"

#define COLOR(hex) pu::ui::Color::FromHex(hex)

namespace inst::ui {
    extern MainApplication *mainApp;

    InstallPage::InstallPage(unordered_map<string,string> installMap, NcmStorageId storageId, function<shared_ptr<tin::install::Install>(string,NcmStorageId)> GetTask, function<void()> OnFinished) : Layout::Layout(), m_taskMap(installMap), m_storageId(storageId), m_cbTask(GetTask), m_cbFinish(OnFinished) {
        this->SetBackgroundColor(COLOR("#670000FF"));
        if (std::filesystem::exists(inst::config::appDir + "/background.png")) this->SetBackgroundImage(inst::config::appDir + "/background.png");
        else this->SetBackgroundImage("romfs:/images/background.jpg");
        this->topRect = Rectangle::New(0, 0, 1280, 94, COLOR("#170909FF"));
        this->infoRect = Rectangle::New(0, 95, 1280, 60, COLOR("#17090980"));
        this->titleImage = Image::New(0, 0, "romfs:/images/logo.png");
        this->appVersionText = TextBlock::New(480, 49, "v" + inst::config::appVersion, 22);
        this->appVersionText->SetColor(COLOR("#FFFFFFFF"));
        this->pageInfoText = TextBlock::New(10, 109, "", 30);
        this->pageInfoText->SetColor(COLOR("#FFFFFFFF"));
        this->installInfoText = TextBlock::New(15, 568, "", 22);
        this->installInfoText->SetColor(COLOR("#FFFFFFFF"));
        this->installBar = pu::ui::elm::ProgressBar::New(10, 600, 800, 40, 100.0f);
        this->installBar->SetColor(COLOR("#222222FF"));
        if (std::filesystem::exists(inst::config::appDir + "/awoo_inst.png")) this->awooImage = Image::New(410, 190, inst::config::appDir + "/awoo_inst.png");
        else this->awooImage = Image::New(510, 166, "romfs:/images/awoos/7d8a05cddfef6da4901b20d2698d5a71.png");
        this->Add(this->topRect);
        this->Add(this->infoRect);
        this->Add(this->titleImage);
        this->Add(this->appVersionText);
        this->Add(this->pageInfoText);
        this->Add(this->installInfoText);
        this->Add(this->installBar);
        this->Add(this->awooImage);
        if (inst::config::gayMode) this->awooImage->SetVisible(false);
    }

    void InstallPage::OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos) {
        if (Down)
            mainApp->PopLayout();
    }

    void InstallPage::OnStart() {
        /* Init install, set boost */
        inst::util::initInstallServices();
        if (appletGetAppletType() == AppletType_Application || appletGetAppletType() == AppletType_SystemApplication) appletBeginBlockingHomeButton(0);

        std::vector<int> previousClockValues;
        if (inst::config::overClock) {
            previousClockValues.push_back(inst::util::setClockSpeed(0, 1785000000)[0]);
            previousClockValues.push_back(inst::util::setClockSpeed(1, 76800000)[0]);
            previousClockValues.push_back(inst::util::setClockSpeed(2, 1600000000)[0]);
        }

        string currentTitleName;

        /* Start installation */
        try {
            for (pair<string,string> urlPair: m_taskMap) {
                currentTitleName = urlPair.second;
                /* Display Info */
                installInfoText->SetText("Installing " + urlPair.second);
                installBar->SetProgress(0);
                mainApp->CallForRender();
                /* Install title */
                auto installTask = m_cbTask(urlPair.first, m_storageId);
                installTask->Prepare();
                installTask->Begin();
            }
        } catch (std::exception& e) {
            LOG_DEBUG("Failed to install");
            LOG_DEBUG("%s", e.what());
            fprintf(stdout, "%s", e.what());
            installInfoText->SetText("Failed to install " + currentTitleName);
            installBar->SetProgress(0);
            inst::ui::mainApp->CreateShowDialog("Failed to install " + currentTitleName + "!", "Partially installed contents can be removed from the System Settings applet.\n\n" + (std::string)e.what(), {"OK"}, true);
        }

        /* Clean up */
        if (appletGetAppletType() == AppletType_Application || appletGetAppletType() == AppletType_SystemApplication) appletEndBlockingHomeButton();
        inst::util::deinitInstallServices();
        if (previousClockValues.size() > 0) {
            inst::util::setClockSpeed(0, previousClockValues[0]);
            inst::util::setClockSpeed(1, previousClockValues[1]);
            inst::util::setClockSpeed(2, previousClockValues[2]);
        }
        installInfoText->SetText("Install complete");
        installBar->SetProgress(100);
        mainApp->CallForRender();

        /* Notify origin */
        m_cbFinish();
    }

    bool InstallPage::OnStop() {
        return true;
    }

    void InstallPage::OnTick() {}
}

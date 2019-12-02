#include "ui/NetWaitPage.hpp"
#include "ui/MainApplication.hpp"
#include "ui/NetInstallPage.hpp"
#include "util/config.hpp"
#include "util/util.hpp"
#include "util/lang.hpp"
#include "util/error.hpp"
#include "netInstall.hpp"

#define COLOR(hex) pu::ui::Color::FromHex(hex)

namespace inst::ui {
    extern MainApplication *mainApp;

    NetWaitPage::NetWaitPage() : Layout::Layout() {
        this->SetBackgroundColor(COLOR("#670000FF"));
        if (std::filesystem::exists(inst::config::appDir + "/background.png")) this->SetBackgroundImage(inst::config::appDir + "/background.png");
        else this->SetBackgroundImage("romfs:/background.jpg");
        this->topRect = Rectangle::New(0, 0, 1280, 94, COLOR("#170909FF"));
        this->infoRect = Rectangle::New(0, 95, 1280, 60, COLOR("#17090980"));
        this->botRect = Rectangle::New(0, 660, 1280, 60, COLOR("#17090980"));
        this->titleImage = Image::New(0, 0, "romfs:/logo.png");
        this->appVersionText = TextBlock::New(480, 49, "v" + inst::config::appVersion, 22);
        this->appVersionText->SetColor(COLOR("#FFFFFFFF"));
        this->butText = TextBlock::New(10, 678, "", 24);
        this->butText->SetColor(COLOR("#FFFFFFFF"));
        this->butText->SetText("inst.net.buttons"_lang);
        this->pageInfoText = TextBlock::New(10, 109, "", 30);
        this->pageInfoText->SetColor(COLOR("#FFFFFFFF"));
        this->infoImage = Image::New(453, 292, "romfs:/lan-connection-waiting.png");
        this->Add(this->topRect);
        this->Add(this->infoRect);
        this->Add(this->botRect);
        this->Add(this->titleImage);
        this->Add(this->appVersionText);
        this->Add(this->butText);
        this->Add(this->pageInfoText);
        this->Add(this->infoImage);
    }

    void NetWaitPage::OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos) {
        if (Down & KEY_B)
            mainApp->PopLayout();
        if (Down & KEY_Y) {
            switch (
                mainApp->CreateShowDialog(
                    "inst.net.src.title"_lang,
                    "inst.net.src.content"_lang,
                    {
                        "inst.net.src.opt0"_lang,
                        "inst.net.src.opt1"_lang
                    },
                    false,
                    850)
                ) {
                case 0:
                    this->EnterURL();
                    return;
                case 1:
                    this->EnterGdrive();
                    return;
            }
        }
        if (Down & KEY_X) {
            mainApp->CreateShowDialog(
                "inst.net.help.title"_lang,
                "inst.net.help.content"_lang,
                { "inst.net.help.opt0"_lang },
                true,
                850
            );
        }
    }

    void NetWaitPage::OnStart() {
        Result rc = net::InitializeServerSocket();
        if (R_SUCCEEDED(rc)) {
            this->pageInfoText->SetText(inst::util::getIPAddress());
        } else {
            this->pageInfoText->SetText("common.failed"_lang);
        }
    }

    bool NetWaitPage::OnStop() {
        net::CloseServerSocket();
        return true;
    }

    void NetWaitPage::OnTick() {
        std::vector<std::string> urls = net::GetUrls();
        if (urls.size() > 0) {
            std::unordered_map<std::string,std::string> urlMap;
            for (std::string url: urls)
                urlMap.insert( { url, inst::util::shortenString(inst::util::formatUrlString(url), 32, true) } );
            mainApp->LoadLayout(
                NetInstallPage::New(
                    urlMap,
                    " over local network"
                )
            );
        }
    }

    void NetWaitPage::EnterURL() {
        std::string keyboardResult = inst::util::softwareKeyboard("inst.net.url_hint"_lang, lastUrl, 500);
        if (keyboardResult.size() == 0)
            return;
        lastUrl = keyboardResult;
        if (inst::util::formatUrlString(keyboardResult) == "" || keyboardResult == "https://" || keyboardResult == "http://") {
            mainApp->CreateShowDialog(
                "The URL specified is invalid!",
                "",
                {"OK"},
                false
            );
            return;
        }
        std::unordered_map<std::string,std::string> urlMap({ { keyboardResult, keyboardResult } });
        mainApp->LoadLayout(
            NetInstallPage::New(
                urlMap,
                " from URL"
            )
        );
    }

    void NetWaitPage::EnterGdrive() {
        std::string keyboardResult = inst::util::softwareKeyboard("inst.net.gdrive_hint"_lang, lastFileID, 50);
        if (keyboardResult.size() == 0)
            return;
        lastFileID = keyboardResult;
        std::string fileName = inst::util::getDriveFileName(keyboardResult);
        if (fileName.size() == 0)
            fileName = "Google Drive File";
        std::unordered_map<std::string,std::string> urlMap(
            { {
                "https://www.googleapis.com/drive/v3/files/" + keyboardResult + "?key=" + inst::config::gAuthKey + "&alt=media",
                fileName
            } });
        mainApp->LoadLayout(
            NetInstallPage::New(
                urlMap,
                " from URL"
            )
        );
    }
}
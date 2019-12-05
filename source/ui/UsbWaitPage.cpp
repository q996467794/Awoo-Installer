#include "ui/UsbWaitPage.hpp"
#include "ui/MainApplication.hpp"
#include "ui/UsbInstallPage.hpp"
#include "usbInstall.hpp"
#include "util/config.hpp"
#include "util/util.hpp"
#include "util/lang.hpp"

#define COLOR(hex) pu::ui::Color::FromHex(hex)

namespace inst::ui {
    extern MainApplication *mainApp;

    UsbWaitPage::UsbWaitPage() : Layout::Layout() {
        this->SetBackgroundColor(COLOR("#670000FF"));
        if (std::filesystem::exists(inst::config::appDir + "/background.png")) this->SetBackgroundImage(inst::config::appDir + "/background.png");
        else this->SetBackgroundImage("romfs:/images/background.jpg");
        this->topRect = Rectangle::New(0, 0, 1280, 94, COLOR("#170909FF"));
        this->infoRect = Rectangle::New(0, 95, 1280, 60, COLOR("#17090980"));
        this->botRect = Rectangle::New(0, 660, 1280, 60, COLOR("#17090980"));
        this->titleImage = Image::New(0, 0, "romfs:/images/logo.png");
        this->appVersionText = TextBlock::New(480, 49, "v" + inst::config::appVersion, 22);
        this->appVersionText->SetColor(COLOR("#FFFFFFFF"));
        this->butText = TextBlock::New(10, 678, "", 24);
        this->butText->SetColor(COLOR("#FFFFFFFF"));
        this->butText->SetText("inst.usb.buttons"_lang);
        this->pageInfoText = TextBlock::New(10, 109, "", 30);
        this->pageInfoText->SetColor(COLOR("#FFFFFFFF"));
        this->infoImage = Image::New(453, 292, "romfs:/images/icons/lan-connection-waiting.png");
        this->Add(this->topRect);
        this->Add(this->infoRect);
        this->Add(this->botRect);
        this->Add(this->titleImage);
        this->Add(this->appVersionText);
        this->Add(this->butText);
        this->Add(this->pageInfoText);
        this->Add(this->infoImage);

        this->m_future = async(usb::GetFileList);
    }

    void UsbWaitPage::OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos) {
        if (Down & KEY_B)
            mainApp->PopLayout();
        if (Down & KEY_X) {
            mainApp->CreateShowDialog(
                "inst.usb.help.title"_lang,
                "inst.usb.help.content"_lang,
                { "inst.usb.help.opt0"_lang },
                true,
                850
            );
        }
    }

    void UsbWaitPage::OnStart() {
    }

    bool UsbWaitPage::OnStop() {
        return true;
    }

    void UsbWaitPage::OnTick() {
        std::chrono::milliseconds span(10);
        if (m_future.wait_for(span) == std::future_status::timeout)
            return;
        std::vector<std::string> urls = m_future.get();
        if (urls.size() > 0) {
            std::unordered_map<std::string,std::string> urlMap;
            for (std::string url: urls)
                urlMap.insert( { url, inst::util::shortenString(inst::util::formatUrlString(url), 32, true) } );
            mainApp->LoadLayout(
                UsbInstallPage::New(
                    urlMap,
                    " over local network"
                )
            );
        }
    }
}
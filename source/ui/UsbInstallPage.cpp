#include "ui/MainApplication.hpp"
#include "ui/UsbInstallPage.hpp"
#include "ui/InstallPage.hpp"

#include "util/util.hpp"
#include "util/lang.hpp"
#include "util/config.hpp"
#include "usbInstall.hpp"

#define COLOR(hex) pu::ui::Color::FromHex(hex)

namespace inst::ui {
    extern MainApplication *mainApp;

    UsbInstallPage::UsbInstallPage(unordered_map<string, string> links, string source) : Layout::Layout(), m_links(links), m_source(source) {
        this->SetBackgroundColor(COLOR("#670000FF"));
        this->SetBackgroundImage("romfs:/images/background.jpg");
        this->topRect = Rectangle::New(0, 0, 1280, 94, COLOR("#170909FF"));
        this->infoRect = Rectangle::New(0, 95, 1280, 60, COLOR("#17090980"));
        this->botRect = Rectangle::New(0, 660, 1280, 60, COLOR("#17090980"));
        this->titleImage = Image::New(0, 0, "romfs:/images/logo.png");
        this->appVersionText = TextBlock::New(480, 49, "v" + inst::config::appVersion, 22);
        this->appVersionText->SetColor(COLOR("#FFFFFFFF"));
        this->pageInfoText = TextBlock::New(10, 109, "", 30);
        this->pageInfoText->SetColor(COLOR("#FFFFFFFF"));
        this->butText = TextBlock::New(10, 678, "", 24);
        this->butText->SetColor(COLOR("#FFFFFFFF"));
        this->butText->SetText("\ue0e0 Select File    \ue0e3 Select All    \ue0ef Install File(s)    \ue0e1 Cancel ");
        this->menu = pu::ui::elm::Menu::New(0, 156, 1280, COLOR("#FFFFFF00"), 84, (506 / 84));
        this->menu->SetOnFocusColor(COLOR("#00000033"));
        this->menu->SetScrollbarColor(COLOR("#17090980"));
        this->infoImage = Image::New(460, 332, "romfs:/images/icons/usb-connection-waiting.png");
        this->Add(this->topRect);
        this->Add(this->infoRect);
        this->Add(this->botRect);
        this->Add(this->titleImage);
        this->Add(this->appVersionText);
        this->Add(this->butText);
        this->Add(this->pageInfoText);
        this->Add(this->menu);
        this->Add(this->infoImage);

        for (pair<string, string> urlPair: m_links) {
            auto item = MenuItem::New(urlPair.second);
            item->SetColor(COLOR("#FFFFFFFF"));
            item->SetIcon(inst::util::GetMenuOptionIcon(false));
            item->AddOnClick(bind(&UsbInstallPage::OnSelect, this, item, urlPair));
            this->menu->AddItem(item);
        }
    }

    void UsbInstallPage::OnSelect(MenuItem::Ref &item, pair<string, string> urlPair) {
        auto it = find(m_selected.begin(), m_selected.end(), urlPair.first);
        if (it != m_selected.end()) { /* contained selected url */
            m_selected.erase(it);
            item->SetIcon(inst::util::GetMenuOptionIcon(false));
        } else { /* didn't contain slected url */
            m_selected.push_back(urlPair.first);
            item->SetIcon(inst::util::GetMenuOptionIcon(true));
        }
        this->menu->ReloadItemRenders();
    }

    void UsbInstallPage::StartInstall() {
        int rc = mainApp->CreateShowDialog(
            "inst.target.title"_lang,
            this->m_selected.size() == 1?
                "inst.target.content0"_lang + inst::util::shortenString(inst::util::formatUrlString(this->m_selected[0]), 32, true) + "inst.target.content1"_lang:
                "inst.target.contents0"_lang + to_string(this->m_selected.size()) + "inst.target.contents1"_lang,
            { "common.sd"_lang, "common.nand"_lang },
            false
        );
        if (rc < 0)
            return;
        
        NcmStorageId storageId;
        if (rc == 0) {
            storageId = NcmStorageId_SdCard;
        } else {
            storageId = NcmStorageId_BuiltInUser;
        }

        unordered_map<string,string> selectedMap;
        for (filesystem::path path: this->m_selected)
            selectedMap.insert( pair(path.string(), path.filename()));
        
        mainApp->LoadLayout(InstallPage::New(selectedMap, storageId, usb::GetUsbTask, usb::nop));
    }

    void UsbInstallPage::OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos) {
        if (Down & KEY_B) {
            mainApp->PopLayout();
        }
        if (Down & KEY_PLUS)
            if (m_selected.size() > 0)
                this->StartInstall();
    }

    void UsbInstallPage::OnStart() {}

    bool UsbInstallPage::OnStop() {
        return true;
    }

    void UsbInstallPage::OnTick() {}
}
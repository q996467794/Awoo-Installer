#include "ui/NetInstallPage.hpp"

#include "ui/MainApplication.hpp"
#include "InstallPage.hpp"

#include "util/config.hpp"
#include "util/lang.hpp"
#include "util/util.hpp"
#include "util/curl.hpp"
#include "netInstall.hpp"

#include <iostream>

#define COLOR(hex) pu::ui::Color::FromHex(hex)

namespace inst::ui {
    extern MainApplication *mainApp;

    NetInstallPage::NetInstallPage(unordered_map<string, string> links, string source) : Layout::Layout(), m_links(links), m_source(source) {
        this->SetBackgroundColor(COLOR("#670000FF"));
        if (filesystem::exists(inst::config::appDir + "/background.png")) this->SetBackgroundImage(inst::config::appDir + "/background.png");
        else this->SetBackgroundImage("romfs:/images/background.jpg");
        this->topRect = Rectangle::New(0, 0, 1280, 94, COLOR("#170909FF"));
        this->infoRect = Rectangle::New(0, 95, 1280, 60, COLOR("#17090980"));
        this->botRect = Rectangle::New(0, 660, 1280, 60, COLOR("#17090980"));
        this->titleImage = Image::New(0, 0, "romfs:/images/logo.png");
        this->appVersionText = TextBlock::New(480, 49, "v" + inst::config::appVersion, 22);
        this->appVersionText->SetColor(COLOR("#FFFFFFFF"));
        this->butText = TextBlock::New(10, 678, "", 24);
        this->butText->SetColor(COLOR("#FFFFFFFF"));
        this->butText->SetText("inst.net.buttons"_lang);
        this->pageInfoText = TextBlock::New(10, 109, "", 30);
        this->pageInfoText->SetColor(COLOR("#FFFFFFFF"));
        this->menu = Menu::New(0, 156, 1280, COLOR("#FFFFFF00"), 84, (506 / 84));
        this->menu->SetOnFocusColor(COLOR("#00000033"));
        this->menu->SetScrollbarColor(COLOR("#17090980"));
        this->Add(this->topRect);
        this->Add(this->infoRect);
        this->Add(this->botRect);
        this->Add(this->titleImage);
        this->Add(this->appVersionText);
        this->Add(this->butText);
        this->Add(this->pageInfoText);
        this->Add(this->menu);

        for (pair<string, string> urlPair: m_links) {
            auto item = MenuItem::New(urlPair.second);
            item->SetColor(COLOR("#FFFFFFFF"));
            item->SetIcon(inst::util::GetMenuOptionIcon(false));
            item->AddOnClick(bind(&NetInstallPage::OnSelect, this, item, urlPair));
            this->menu->AddItem(item);
        }
    }

    void NetInstallPage::OnSelect(MenuItem::Ref &item, pair<string, string> urlPair) {
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

    void NetInstallPage::StartInstall() {
        int rc = mainApp->CreateShowDialog(
            "inst.target.title"_lang,
            m_selected.size() == 1?
                "inst.target.content0"_lang + m_links[m_selected[0]] + "inst.target.content1"_lang:
                "inst.target.contents0"_lang + to_string(m_selected.size()) + "inst.target.contents1"_lang,
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
        for (string sel: m_selected) {
            selectedMap.insert({ sel, m_links[sel] });
        }

        mainApp->LoadLayout(InstallPage::New(selectedMap, storageId, net::GetNetTask, net::SendAck));
    }

    void NetInstallPage::OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos) {
        if (Down & KEY_B)
            mainApp->PopLayout();
        if (Down & KEY_PLUS)
            if (m_selected.size() > 0)
                this->StartInstall();
    }

    void NetInstallPage::OnStart() {}

    bool NetInstallPage::OnStop() {
        return true;
    }

    void NetInstallPage::OnTick() {}
}
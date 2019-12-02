#include "ui/MainApplication.hpp"
#include "ui/SdInstPage.hpp"
#include "ui/InstallPage.hpp"

#include "util/util.hpp"
#include "util/config.hpp"
#include "util/lang.hpp"
#include "sdInstall.hpp"

#include <filesystem>

#define COLOR(hex) pu::ui::Color::FromHex(hex)

namespace inst::ui {
    extern MainApplication *mainApp;

    SdInstPage::SdInstPage() : Layout::Layout() {
        this->SetBackgroundColor(COLOR("#670000FF"));
        if (std::filesystem::exists(inst::config::appDir + "/background.png")) this->SetBackgroundImage(inst::config::appDir + "/background.png");
        else this->SetBackgroundImage("romfs:/background.jpg");
        this->topRect = Rectangle::New(0, 0, 1280, 94, COLOR("#170909FF"));
        this->infoRect = Rectangle::New(0, 95, 1280, 60, COLOR("#17090980"));
        this->botRect = Rectangle::New(0, 660, 1280, 60, COLOR("#17090980"));
        this->titleImage = Image::New(0, 0, "romfs:/logo.png");
        this->appVersionText = TextBlock::New(480, 49, "v" + inst::config::appVersion, 22);
        this->appVersionText->SetColor(COLOR("#FFFFFFFF"));
        this->pageInfoText = TextBlock::New(10, 109, "Select what files you want to install, then press the Plus button!", 30);
        this->pageInfoText->SetColor(COLOR("#FFFFFFFF"));
        this->butText = TextBlock::New(10, 678, "\ue0e0 Select File    \ue0e3 Select All    \ue0ef Install File(s)    \ue0e2 Help    \ue0e1 Cancel ", 24);
        this->butText->SetColor(COLOR("#FFFFFFFF"));
        this->menu = pu::ui::elm::Menu::New(0, 156, 1280, COLOR("#FFFFFF00"), 84, (506 / 84));
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

        this->drawMenuItems(true, "sdmc:/");
    }

    void SdInstPage::drawMenuItems(bool clearItems, std::filesystem::path ourPath) {
        if (clearItems) this->selectedTitles = {};
        if (ourPath == "sdmc:") this->currentDir = std::filesystem::path(ourPath.string() + "/");
        else this->currentDir = ourPath;
        this->menu->ClearItems();
        try {
            this->ourDirectories = util::getDirsAtPath(this->currentDir);
            this->ourFiles = util::getDirectoryFiles(this->currentDir, {".nsp", ".nsz", ".xci", ".xcz"});
        } catch (std::exception& e) {
            this->drawMenuItems(false, this->currentDir.parent_path());
            return;
        }
        if (this->currentDir != "sdmc:/") {
            pu::String itm = "..";
            auto ourEntry = pu::ui::elm::MenuItem::New(itm);
            ourEntry->SetColor(COLOR("#FFFFFFFF"));
            ourEntry->SetIcon("romfs:/folder-upload.png");
            this->menu->AddItem(ourEntry);
        }
        for (auto& file: this->ourDirectories) {
            if (file == "..") break;
            pu::String itm = file.filename().string();
            auto ourEntry = pu::ui::elm::MenuItem::New(itm);
            ourEntry->SetColor(COLOR("#FFFFFFFF"));
            ourEntry->SetIcon("romfs:/folder.png");
            this->menu->AddItem(ourEntry);
        }
        for (auto& file: this->ourFiles) {
            pu::String itm = file.filename().string();
            auto ourEntry = pu::ui::elm::MenuItem::New(itm);
            ourEntry->SetColor(COLOR("#FFFFFFFF"));
            ourEntry->SetIcon("romfs:/checkbox-blank-outline.png");
            for (long unsigned int i = 0; i < this->selectedTitles.size(); i++) {
                if (this->selectedTitles[i] == file) {
                    ourEntry->SetIcon("romfs:/check-box-outline.png");
                }
            }
            this->menu->AddItem(ourEntry);
        }
    }

    void SdInstPage::followDirectory() {
        int selectedIndex = this->menu->GetSelectedIndex();
        int dirListSize = this->ourDirectories.size();
        if (this->currentDir != "sdmc:/") {
            dirListSize++;
            selectedIndex--;
        }
        if (selectedIndex < dirListSize) {
            if (this->menu->GetItems()[this->menu->GetSelectedIndex()]->GetName() == ".." && this->menu->GetSelectedIndex() == 0) {
                this->drawMenuItems(true, this->currentDir.parent_path());
            } else {
                this->drawMenuItems(true, this->ourDirectories[selectedIndex]);
            }
            this->menu->SetSelectedIndex(0);
        }
    }

    void SdInstPage::selectNsp(int selectedIndex) {
        int dirListSize = this->ourDirectories.size();
        if (this->currentDir != "sdmc:/") dirListSize++;
        if (this->menu->GetItems()[selectedIndex]->GetIcon() == "romfs:/check-box-outline.png") {
            for (long unsigned int i = 0; i < this->selectedTitles.size(); i++) {
                if (this->selectedTitles[i] == this->ourFiles[selectedIndex - dirListSize]) this->selectedTitles.erase(this->selectedTitles.begin() + i);
            }
        } else if (this->menu->GetItems()[selectedIndex]->GetIcon() == "romfs:/checkbox-blank-outline.png") this->selectedTitles.push_back(this->ourFiles[selectedIndex - dirListSize]);
        else {
            this->followDirectory();
            return;
        }
        this->drawMenuItems(false, currentDir);
    }

    void SdInstPage::startInstall() {
        int rc = mainApp->CreateShowDialog(
            "inst.target.title"_lang,
            this->selectedTitles.size() == 1?
                "inst.target.content0"_lang + inst::util::shortenString(std::filesystem::path(this->selectedTitles[0]).filename().string(), 32, true) + "inst.target.content1"_lang:
                "inst.target.contents0"_lang + to_string(this->selectedTitles.size()) + "inst.target.contents1"_lang,
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
        for (filesystem::path path: this->selectedTitles)
            selectedMap.insert( pair(path.string(), path.filename()));
        
        mainApp->LoadLayout(InstallPage::New(selectedMap, storageId, sd::GetSdTask, sd::nop));
    }

    void SdInstPage::OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos) {
        if (Down & KEY_B) {
            mainApp->PopLayout();
        }
        if ((Down & KEY_A) || (Up & KEY_TOUCH)) {
            this->selectNsp(this->menu->GetSelectedIndex());
            if (this->ourFiles.size() == 1 && this->selectedTitles.size() == 1) {
                this->startInstall();
            }
        }
        if ((Down & KEY_Y)) {
            if (this->selectedTitles.size() == this->ourFiles.size()) this->drawMenuItems(true, currentDir);
            else {
                int topDir = 0;
                if (this->currentDir != "sdmc:/") topDir++;
                for (long unsigned int i = this->ourDirectories.size() + topDir; i < this->menu->GetItems().size(); i++) {
                    if (this->menu->GetItems()[i]->GetIcon() == "romfs:/check-box-outline.png") continue;
                    else this->selectNsp(i);
                }
                this->drawMenuItems(false, currentDir);
            }
        }
        if ((Down & KEY_X)) {
            inst::ui::mainApp->CreateShowDialog("Help", "Copy your NSP, NSZ, XCI, or XCZ files to your SD card, browse to and\nselect the ones you want to install, then press the Plus button.", {"OK"}, true);
        }
        if (Down & KEY_PLUS) {
            if (this->selectedTitles.size() == 0 && this->menu->GetItems()[this->menu->GetSelectedIndex()]->GetIcon() == "romfs:/checkbox-blank-outline.png") {
                this->selectNsp(this->menu->GetSelectedIndex());
            }
            if (this->selectedTitles.size() > 0) this->startInstall();
        }
    }

    void SdInstPage::OnStart() {}

    bool SdInstPage::OnStop() {
        return true;
    }

    void SdInstPage::OnTick() {}
}

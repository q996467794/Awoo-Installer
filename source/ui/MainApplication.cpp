#include "ui/MainApplication.hpp"

namespace inst::ui {
    MainApplication *mainApp;

    void MainApplication::OnLoad() {
        mainApp = this;

        this->mainPage = MainPage::New();
        /*this->netinstPage = netInstPage::New();
        this->sdinstPage = sdInstPage::New();
        this->usbinstPage = usbInstPage::New();
        this->instpage = instPage::New();
        this->optionspage = optionsPage::New();*/
        this->LoadLayout(this->mainPage);
    }
}
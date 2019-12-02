#pragma once
#include "install/install.hpp"
#include <string>

using namespace std;

namespace usb {
    std::vector<std::string> GetFileList();
    shared_ptr<tin::install::Install> GetUsbTask(string path, NcmStorageId destStorage);
    inline void nop() {}
}
#pragma once
#include "install/install.hpp"
#include <string>

using namespace std;

namespace sd {
    shared_ptr<tin::install::Install> GetSdTask(string path, NcmStorageId destStorage);
    inline void nop() {}
}
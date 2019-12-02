/*
Copyright (c) 2017-2018 Adubbz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "sdInstall.hpp"

#include "install/install_nsp.hpp"
#include "install/install_xci.hpp"
#include "install/sd_xci.hpp"
#include "install/sd_nsp.hpp"

#include "util/config.hpp"
#include "util/util.hpp"
#include "util/lang.hpp"
#include "util/error.hpp"

#include <cstring>
#include <sstream>
#include <filesystem>
#include <ctime>

namespace sd {
    shared_ptr<tin::install::Install> GetSdTask(string path, NcmStorageId destStorage) {
        if (path.substr(path.length() - 3, 2) == "xc") {
            auto sdXCI = new tin::install::xci::LocalXCI(path);
            return shared_ptr<tin::install::xci::XCIInstallTask>(new tin::install::xci::XCIInstallTask(destStorage, inst::config::ignoreReqVers, sdXCI));
        } else {
            auto sdNSP = new tin::install::nsp::SdNSP(path);
            return shared_ptr<tin::install::nsp::NSPInstall>(new tin::install::nsp::NSPInstall(destStorage, inst::config::ignoreReqVers, sdNSP));
        }
    }
}

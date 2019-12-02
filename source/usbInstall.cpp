#include "usbInstall.hpp"

#include "util/error.hpp"
#include "util/usb_util.hpp"
#include "util/util.hpp"
#include "util/config.hpp"

#include "install/usb_nsp.hpp"
#include "install/usb_xci.hpp"
#include "install/install_nsp.hpp"
#include "install/install_xci.hpp"

#include <string>

namespace usb {
    struct TUSHeader
    {
        u32 magic; // TUL0 (Tinfoil Usb List 0)
        u32 titleListSize;
        u64 padding;
    } PACKED;

    std::vector<std::string> GetFileList() {
        Result rc = 0;

        while(true) {
            rc = usbDsWaitReady(1000000);
            if (R_SUCCEEDED(rc)) break;
            else if ((rc & 0x3FFFFF) != 0xEA01)
                return {};
        }

        TUSHeader header;
        tin::util::USBRead(&header, sizeof(TUSHeader));

        if (header.magic != 0x304C5554)
            return {};

        auto titleListBuf = std::make_unique<char[]>(header.titleListSize+1);
        std::vector<std::string> titleNames;
        memset(titleListBuf.get(), 0, header.titleListSize+1);

        tin::util::USBRead(titleListBuf.get(), header.titleListSize);

        // Split the string up into individual title names
        std::stringstream titleNamesStream(titleListBuf.get());
        std::string segment;
        while (std::getline(titleNamesStream, segment, '\n')) {
            titleNames.push_back(segment);
        }

        return titleNames;
    }

    shared_ptr<tin::install::Install> GetUsbTask(string path, NcmStorageId destStorage) {
        if (path.substr(path.length() - 3, 2) == "xc") {
            auto usbXCI = new tin::install::xci::USBXCI(path);
            return shared_ptr<tin::install::xci::XCIInstallTask>(new tin::install::xci::XCIInstallTask(destStorage, inst::config::ignoreReqVers, usbXCI));
        } else {
            auto usbNSP = new tin::install::nsp::USBNSP(path);
            return shared_ptr<tin::install::nsp::NSPInstall>(new tin::install::nsp::NSPInstall(destStorage, inst::config::ignoreReqVers, usbNSP));
        }
    }
}